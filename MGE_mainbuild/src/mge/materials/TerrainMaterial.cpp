#include <glm.hpp>

#include "mge/materials/TerrainMaterial.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/config.hpp"

ShaderProgram* TerrainMaterial::_shader = NULL;

TerrainMaterial::TerrainMaterial(Texture* height,Texture* splat,Texture* diff0,Texture* diff1,Texture* diff2,Texture* diff3):
    _heightMap(height), _splatMap(splat), _diffuse0(diff0),_diffuse1(diff1),_diffuse2(diff2),_diffuse3(diff3)
{
    _lazyInitializeShader();
}

TerrainMaterial::~TerrainMaterial() {}

void TerrainMaterial::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH+"terrain.vs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH+"terrain.fs");
        _shader->finalize();
    }
}

void TerrainMaterial::render(GameObject* pGameObject, Camera* pCamera) {
    if (!(_heightMap&&_splatMap&&_diffuse0&&_diffuse1&&_diffuse2&&_diffuse3)) return;

    _shader->use();



    //setup texture slots
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _heightMap->getId());
    glUniform1i (_shader->getUniformLocation("heightMap"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _splatMap->getId());
    glUniform1i (_shader->getUniformLocation("splatMap"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _diffuse0->getId());
    glUniform1i (_shader->getUniformLocation("diffuse0"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _diffuse1->getId());
    glUniform1i (_shader->getUniformLocation("diffuse1"), 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _diffuse2->getId());
    glUniform1i (_shader->getUniformLocation("diffuse2"), 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _diffuse3->getId());
    glUniform1i (_shader->getUniformLocation("diffuse3"), 5);

    //pass in all MVP matrices separately
    glUniformMatrix4fv ( _shader->getUniformLocation("projectionMatrix"),   1, GL_FALSE, glm::value_ptr(pCamera->getProjection()));
    glUniformMatrix4fv ( _shader->getUniformLocation("viewMatrix"),         1, GL_FALSE, glm::value_ptr(pCamera->getView()));
    glUniformMatrix4fv ( _shader->getUniformLocation("modelMatrix"),        1, GL_FALSE, glm::value_ptr(pGameObject->getWorldTransform() ) );

    //now inform mesh of where to stream its data
    pGameObject->getMesh()->streamToOpenGL(
        _shader->getAttribLocation("vertex"),
        _shader->getAttribLocation("normal"),
        _shader->getAttribLocation("uv")
    );
}
