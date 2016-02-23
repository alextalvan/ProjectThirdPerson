#include "mge/materials/ShadowCubeMaterial.hpp"
#include <glm.hpp>
#include "mge/core/Mesh.hpp"
#include "mge/config.hpp"
#include "mge/core/Light.hpp"
#include <string>

ShaderProgram* ShadowCubeMaterial::_shader = NULL;
GLint ShadowCubeMaterial::_modelMatLoc = 0;

ShadowCubeMaterial::ShadowCubeMaterial(int pScreenWidth, int pScreenHeight) : _screenWidth(pScreenWidth), _screenHeight(pScreenHeight) {
    _lazyInitializeShader();
}

ShadowCubeMaterial::~ShadowCubeMaterial() {}

void ShadowCubeMaterial::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH+"depth_cube_shader.vs");
        _shader->addShader(GL_GEOMETRY_SHADER, config::MGE_SHADER_PATH+"depth_cube_shader.gs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH+"depth_cube_shader.fs");

        glBindAttribLocation(GL_VERTEX_SHADER, 0, "vertex");

        glGetError();

        _shader->finalize();

        _modelMatLoc = _shader->getUniformLocation("modelMatrix");
    }
}

void ShadowCubeMaterial::render(World* pWorld, GameObject* pGameObject, Light* light) {
    _shader->use();

    GLfloat aspect = (GLfloat)_screenWidth/(GLfloat)_screenHeight;
    GLfloat near = 1.0f;
    GLfloat far = 25.0f;
    glm::mat4 shadowProj = glm::perspective(90.0f, aspect, near, far);

    GLuint loc;
    glm::vec3 lightPos = light->getWorldPosition();
    loc = _shader->getUniformLocation("lightPos");
    glUniform3fv(loc,1,glm::value_ptr(lightPos));
    loc = _shader->getUniformLocation("far_plane");
    glUniform1f(loc,far);

    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,0.0,1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,-1.0,0.0), glm::vec3(0.0,0.0,-1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,-1.0,0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0,-1.0,0.0)));

    for (GLuint i = 0; i < 6; ++i)
         glUniformMatrix4fv(_shader->getUniformLocation("shadowMatrices[" + std::to_string(i) + "]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));

    glUniformMatrix4fv(_modelMatLoc, 1, GL_FALSE, glm::value_ptr(pGameObject->getWorldTransform()));

    //now inform mesh of where to stream its data
    pGameObject->getMesh()->streamToOpenGL(0, -1, -1);
}
