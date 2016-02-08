#include <glm.hpp>

#include "mge/materials/TextureMaterial.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/config.hpp"

ShaderProgram* TextureMaterial::_shader = NULL;

TextureMaterial::TextureMaterial(Texture * pDiffuseTexture):_diffuseTexture(pDiffuseTexture) {
    _lazyInitializeShader();
}

TextureMaterial::~TextureMaterial() {}

void TextureMaterial::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH+"texture.vs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH+"texture.fs");
        _shader->finalize();
    }
}

void TextureMaterial::setDiffuseTexture (Texture* pDiffuseTexture) {
    _diffuseTexture = pDiffuseTexture;
}

void TextureMaterial::render(World* pWorld, GameObject* pGameObject, Camera* pCamera) {
    if (!_diffuseTexture) return;

    _shader->use();

    //setup texture slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _diffuseTexture->getId());
    glUniform1i (_shader->getUniformLocation("textureDiffuse"), 0);

    //pass in all MVP matrices separately
    glUniformMatrix4fv ( _shader->getUniformLocation("projectionMatrix"),   1, GL_FALSE, glm::value_ptr(pCamera->getProjection()));
    glUniformMatrix4fv ( _shader->getUniformLocation("viewMatrix"),         1, GL_FALSE, glm::value_ptr(glm::inverse(pCamera->getWorldTransform())));
    glUniformMatrix4fv ( _shader->getUniformLocation("modelMatrix"),        1, GL_FALSE, glm::value_ptr(pGameObject->getWorldTransform() ) );

    //now inform mesh of where to stream its data
    pGameObject->getMesh()->streamToOpenGL(
        _shader->getAttribLocation("vertex"),
        _shader->getAttribLocation("normal"),
        _shader->getAttribLocation("uv")
    );
}
