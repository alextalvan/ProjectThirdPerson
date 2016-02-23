#include <mge/materials/Particles/ParticleMaterial.hpp>


ShaderProgram* ParticleMaterial::_shader = NULL;

ParticleMaterial::ParticleMaterial(Texture * pDiffuseTexture):_diffuseTexture(pDiffuseTexture) {
    _lazyInitializeShader();
    _rotMatrixLoc = _shader->getUniformLocation("rotationMatrix");
    _vpMatrixLoc = _shader->getUniformLocation("vpMatrix");
}

ParticleMaterial::~ParticleMaterial() {}

void ParticleMaterial::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH+"Particles/particle.vs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH+"Particles/particle.fs");
        _shader->finalize();

        //_vpMatrixLoc =
    }
}

void ParticleMaterial::setDiffuseTexture (Texture* pDiffuseTexture) {
    _diffuseTexture = pDiffuseTexture;
}

void ParticleMaterial::render(World* pWorld, GameObject* pGameObject, Camera* pCamera) {
    if (!_diffuseTexture) return;
    using namespace glm;

    _shader->use();

    //setup texture slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _diffuseTexture->getId());
    glUniform1i (_shader->getUniformLocation("textureDiffuse"), 0);


    mat4 vp = pCamera->getProjection() * pCamera->getView();
    mat4 rot = pCamera->getWorldTransform();
    //pass in all MVP matrices separately
    //glUniformMatrix4fv ( _shader->getUniformLocation("vpMatrix"),         1, GL_FALSE, glm::value_ptr(vp));
    //glUniformMatrix4fv ( _shader->getUniformLocation("modelMatrix"),        1, GL_FALSE, glm::value_ptr(pGameObject->getWorldTransform() ) );

    //now inform mesh of where to stream its data
    pGameObject->getMesh()->streamToOpenGL(
        _shader->getAttribLocation("vertex"),
        _shader->getAttribLocation("normal"),
        _shader->getAttribLocation("uv")
    );
}
