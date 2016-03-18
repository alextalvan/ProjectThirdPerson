#include <mge/materials/Particles/ParticleMaterial.hpp>
#include <mge/particles/ParticleSystem.hpp>


ShaderProgram* ParticleMaterial::_shader = NULL;

GLuint ParticleMaterial::_rotMatrixLoc = 0;
GLuint ParticleMaterial::_vpMatrixLoc = 0;
GLuint ParticleMaterial::_posLoc = 0;
GLuint ParticleMaterial::_scaleLoc = 0;
GLuint ParticleMaterial::_lifetimeLoc = 0;
GLuint ParticleMaterial::_texLoc = 0;

GLuint ParticleMaterial::_offsetsBuffer = 0;
GLuint ParticleMaterial::_lifetimesBuffer = 0;
GLuint ParticleMaterial::_scalesBuffer = 0;

ParticleMaterial::ParticleMaterial(Texture * pDiffuseTexture):_diffuseTexture(pDiffuseTexture) {
    _lazyInitializeShader();
    //generate the buffers to be used by particle rendering

}

ParticleMaterial::~ParticleMaterial() {}

void ParticleMaterial::_lazyInitializeShader()
{
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH+"Particles/particle.vs");
        _shader->addShader(GL_GEOMETRY_SHADER, config::MGE_SHADER_PATH+"Particles/particle.gs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH+"Particles/particle.fs");

        _shader->finalize();

        //_vpMatrixLoc =
        _rotMatrixLoc = _shader->getUniformLocation("rotationMatrix");
        _vpMatrixLoc = _shader->getUniformLocation("vpMatrix");
        _posLoc = _shader->getAttribLocation("position");
        _scaleLoc = _shader->getAttribLocation("scale");
        _lifetimeLoc = _shader->getAttribLocation("lifetime");

        _texLoc = _shader->getUniformLocation("textureDiffuse");

        glm::vec3 dummy[MGE_MAX_PARTICLES_PER_SYSTEM];
        glGenBuffers(1, &_offsetsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _offsetsBuffer);
        glBufferData(GL_ARRAY_BUFFER,MGE_MAX_PARTICLES_PER_SYSTEM*sizeof(glm::vec3), &dummy, GL_DYNAMIC_DRAW);


        glm::vec1 dummy2[MGE_MAX_PARTICLES_PER_SYSTEM];
        glGenBuffers(1, &_lifetimesBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _lifetimesBuffer);
        glBufferData(GL_ARRAY_BUFFER, MGE_MAX_PARTICLES_PER_SYSTEM*sizeof(glm::vec1), &dummy2, GL_DYNAMIC_DRAW);
//
//        //float dummy2[MGE_MAX_PARTICLES_PER_SYSTEM];
        glGenBuffers(1, &_scalesBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _scalesBuffer);
        glBufferData(GL_ARRAY_BUFFER, MGE_MAX_PARTICLES_PER_SYSTEM*sizeof(glm::vec1), &dummy2, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void ParticleMaterial::setDiffuseTexture (Texture* pDiffuseTexture) {
    _diffuseTexture = pDiffuseTexture;
}

void ParticleMaterial::render(GameObject* pGameObject, Camera* pCamera)
{
    if (!_diffuseTexture) return;
    using namespace glm;
    //return;
    _shader->use();

    //pass in one ofs
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _diffuseTexture->getId());
    glUniform1i (_texLoc, 0);

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

    //Mesh* quad = pSys->_mesh;
    //quad->enableInOpenGL(_vertLoc,_normalLoc,_uvLoc);

    int particleLimit = pSys->_particleLimit;
    int aliveParticles = 0;

    //insertion of new particles
    for(int i=0;i<MGE_MAX_PARTICLES_PER_SYSTEM;++i)
    {
        if(aliveParticles == particleLimit)
            break;

        if(pSys->_lifeTimesMain[i].x > 0.0f)
        {
            aliveParticles++;
        }
        else
        //check if we can insert a particle from the buffer to the main array
        if(newPartIndex < newPartCount && aliveParticles < particleLimit)
        {
            //pSys->_particles[i] = pSys->_buffer[newPartIndex];//copying by value
            pSys->_lifeTimesMain[i] = pSys->_lifeTimesBuffer[i];
            pSys->_positionsMain[i] = pSys->_positionsBuffer[i];
            pSys->_speedsMain[i] = pSys->_speedsBuffer[i];
            pSys->_scalesMain[i] = pSys->_scalesBuffer[i];

            newPartIndex++;
        }
    }

    pSys->_bufferCount = 0;


    glBindBuffer(GL_ARRAY_BUFFER, _offsetsBuffer);
    glBufferSubData(GL_ARRAY_BUFFER,0,MGE_MAX_PARTICLES_PER_SYSTEM*sizeof(glm::vec3), &(pSys->_positionsMain[0]));
    glEnableVertexAttribArray(_posLoc);
    glVertexAttribPointer(_posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );



    glBindBuffer(GL_ARRAY_BUFFER, _lifetimesBuffer);
    glBufferSubData(GL_ARRAY_BUFFER,0,MGE_MAX_PARTICLES_PER_SYSTEM*sizeof(glm::vec1), &(pSys->_lifeTimesMain[0]));
    glEnableVertexAttribArray(_lifetimeLoc);
    glVertexAttribPointer(_lifetimeLoc, 1, GL_FLOAT, GL_FALSE, 0, (void*)0 );



    glBindBuffer(GL_ARRAY_BUFFER, _scalesBuffer);
    glBufferSubData(GL_ARRAY_BUFFER,0,MGE_MAX_PARTICLES_PER_SYSTEM*sizeof(glm::vec1), &(pSys->_scalesMain[0]));
    glEnableVertexAttribArray(_scaleLoc);
    glVertexAttribPointer(_scaleLoc, 1, GL_FLOAT, GL_FALSE, 0, (void*)0 );



    //THE draw call
    glDrawArrays(GL_POINTS,0,MGE_MAX_PARTICLES_PER_SYSTEM);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(_posLoc);
    glDisableVertexAttribArray(_lifetimeLoc);
    glDisableVertexAttribArray(_scaleLoc);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

}
