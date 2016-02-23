#include "mge/materials/ShadowMaterial.hpp"
#include <glm.hpp>
#include "mge/core/Mesh.hpp"
#include "mge/config.hpp"
#include "mge/core/Light.hpp"
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

void ShadowMaterial::render(World* pWorld, GameObject* pGameObject, Light* light) {
    _shader->use();

    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    GLfloat near_plane = 1.0f, far_plane = 50.0f;
    lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
    lightView = light->getWorldTransform();
    lightSpaceMatrix = lightProjection * lightView;

    glUniformMatrix4fv(_lightMatLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniformMatrix4fv(_modelMatLoc, 1, GL_FALSE, glm::value_ptr(pGameObject->getWorldTransform()));

    //now inform mesh of where to stream its data
    pGameObject->getMesh()->streamToOpenGL(0, -1, -1);
}
