#include "mge/materials/ShadowMaterial.hpp"
#include <glm.hpp>
#include "mge/core/Mesh.hpp"
#include "mge/config.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/Renderer.hpp"
#include <string>

ShaderProgram* ShadowMaterial::_shader = NULL;
GLint ShadowMaterial::_lightMatLoc = 0;
GLint ShadowMaterial::_modelMatLoc = 0;

ShadowMaterial::ShadowMaterial() {
    _lazyInitializeShader();
}

ShadowMaterial::~ShadowMaterial() {}

void ShadowMaterial::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH+"depth_shader.vs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH+"depth_shader.fs");

        glBindAttribLocation(GL_VERTEX_SHADER, 0, "vertex");

        glGetError();

        _shader->finalize();

        _lightMatLoc = _shader->getUniformLocation("lightMatrix");
        _modelMatLoc = _shader->getUniformLocation("modelMatrix");
    }
}

void ShadowMaterial::render(GameObject* pGameObject, Light* light,glm::mat4& projectionMat, bool shrink)
{
    _shader->use();

    //glm::mat4 lightProjection, lightView;
    //glm::mat4 lightSpaceMatrix;
    //GLfloat near_plane = 1.0f, far_plane = 200.0f;

    //float orthoSize = 150.0f;
    //lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
    //lightProjection = Renderer::GetCurrentShadowOrtho();
    //lightView = light->getWorldTransform();
    //lightView = glm::lookAt(light->getWorldPosition(),light->getWorldPosition() + light->getDirection(),glm::vec3(0,1,0));
    //lightView = Light::GetDirectionalViewMatrix();
    //lightSpaceMatrix = projectionMat * Light::GetDirectionalViewMatrix();

//    ShadowCamera* shadowCam = Renderer::GetShadowCamera();
//    lightProjection = shadowCam->getProjection();
//    lightView = shadowCam->getWorldTransform();
//    lightSpaceMatrix = lightProjection * lightView;


    if(shrink)
    {
        glm::mat4 objWorldMat = pGameObject->getWorldTransform();
        objWorldMat[0] *= 0.99f;
        objWorldMat[1] *= 0.99f;
        objWorldMat[2] *= 0.99f;
        glUniformMatrix4fv(_modelMatLoc, 1, GL_FALSE, glm::value_ptr(objWorldMat));
    }
    else
    {
        glUniformMatrix4fv(_modelMatLoc, 1, GL_FALSE, glm::value_ptr(pGameObject->getWorldTransform()));
    }

    glUniformMatrix4fv(_lightMatLoc, 1, GL_FALSE, glm::value_ptr(projectionMat * Light::GetDirectionalViewMatrix()));


    //now inform mesh of where to stream its data
    pGameObject->getMesh()->streamToOpenGL(0, -1, -1);
}
