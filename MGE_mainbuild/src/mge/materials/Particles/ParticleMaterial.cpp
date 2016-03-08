#include <mge/materials/Particles/ParticleMaterial.hpp>
#include <mge/particles/ParticleSystem.hpp>


ShaderProgram* ParticleMaterial::_shader = NULL;

ParticleMaterial::ParticleMaterial(Texture * pDiffuseTexture):_diffuseTexture(pDiffuseTexture) {
    _lazyInitializeShader();

    _rotMatrixLoc = _shader->getUniformLocation("rotationMatrix");
    _vpMatrixLoc = _shader->getUniformLocation("vpMatrix");
    _posVectorLoc = _shader->getUniformLocation("position");
    _scaleVectorLoc = _shader->getUniformLocation("scale");

    _vertLoc = _shader->getAttribLocation("vertex");
    _normalLoc = _shader->getAttribLocation("normal");
    _uvLoc = _shader->getAttribLocation("uv");
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

void ParticleMaterial::render(GameObject* pGameObject, Camera* pCamera) {
    if (!_diffuseTexture) return;
    using namespace glm;
    //return;
    _shader->use();

    //pass in one ofs
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _diffuseTexture->getId());
    glUniform1i (_shader->getUniformLocation("textureDiffuse"), 0);

    mat4 vp = pCamera->getProjection() * pCamera->getView();//pCamera->getProjection() * pCamera->getView();
    mat4 rot = pCamera->getWorldTransform();
    rot[3] = vec4(0,0,0,1);//canceling translation
    glUniformMatrix4fv (_vpMatrixLoc,1, GL_FALSE, glm::value_ptr(vp));
    glUniformMatrix4fv (_rotMatrixLoc,1,GL_FALSE, glm::value_ptr(rot));
    //debug
    //mat4 debug = pCamera->getProjection() * pCamera->getView() * pGameObject->getWorldTransform();
    //glUniformMatrix4fv (_shader->getUniformLocation("debug"),1,GL_FALSE, glm::value_ptr(debug));

    ParticleSystem* pSys = (ParticleSystem*)pGameObject;//we know the object we are rendering is a particle system

    int newPartCount = pSys->_bufferCount;
    int newPartIndex = 0;

    Mesh* quad = pSys->_mesh;
    quad->enableInOpenGL(_vertLoc,_normalLoc,_uvLoc);

    for(int i=0;i<MGE_MAX_PARTICLES_PER_SYSTEM;++i)
    {
        //check if we can insert a particle from the buffer to the main array
        if(pSys->_particles[i].lifeTime < 0.0f && newPartIndex < newPartCount)
        {
            pSys->_particles[i] = pSys->_buffer[newPartIndex];//copying by value

            glUniform3fv(_posVectorLoc,1,glm::value_ptr(pSys->_particles[i].position));
            glUniform2fv(_scaleVectorLoc,1,glm::value_ptr(pSys->_particles[i].scale));
            //draw call
            quad->drawInOpenGL();

            newPartIndex++;
        }
        else
        if(pSys->_particles[i].lifeTime > 0.0f)
        {
            glUniform3fv(_posVectorLoc,1,glm::value_ptr(pSys->_particles[i].position));
            glUniform2fv(_scaleVectorLoc,1,glm::value_ptr(pSys->_particles[i].scale));
            //draw call
            quad->drawInOpenGL();
        }
    }


    pSys->_bufferCount = 0;

    quad->disableInOpenGL(_vertLoc,_normalLoc,_uvLoc);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

}
