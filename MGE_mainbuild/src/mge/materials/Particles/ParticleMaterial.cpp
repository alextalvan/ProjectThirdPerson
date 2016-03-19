#include <mge/materials/Particles/ParticleMaterial.hpp>
#include <mge/particles/ParticleSystem.hpp>
#include <mge/core/Renderer.hpp>


ShaderProgram* ParticleMaterial::_shader = NULL;

GLuint ParticleMaterial::_rotMatrixLoc = 0;
GLuint ParticleMaterial::_vpMatrixLoc = 0;
GLuint ParticleMaterial::_texLoc = 0;


GLuint ParticleMaterial::_posLoc = 0;
GLuint ParticleMaterial::_lifeScaleLoc = 0;


GLuint ParticleMaterial::_offsetsBuffer = 0;
GLuint ParticleMaterial::_lifeScaleBuffer = 0;

std::size_t ParticleMaterial::_vec3Size = sizeof(glm::vec3);
std::size_t ParticleMaterial::_vec2Size = sizeof(glm::vec2);

ParticleMaterial::ParticleMaterial(Texture * pDiffuseTexture):_diffuseTexture(pDiffuseTexture)
{
    _lazyInitializeShader();
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

        _rotMatrixLoc = _shader->getUniformLocation("rotationMatrix");
        _vpMatrixLoc = _shader->getUniformLocation("vpMatrix");
        _posLoc = _shader->getAttribLocation("position");
        _lifeScaleLoc = _shader->getAttribLocation("particleInfo");

        _texLoc = _shader->getUniformLocation("textureDiffuse");

        glm::vec3 dummy_vec3[MGE_MAX_PARTICLES_PER_SYSTEM];
        glm::vec2 dummy_vec2[MGE_MAX_PARTICLES_PER_SYSTEM];

        for(int i=0;i<MGE_MAX_PARTICLES_PER_SYSTEM;++i)
        {
            dummy_vec2[i] = glm::vec2(-1.0f,1.0f);
            dummy_vec3[i] = glm::vec3(-10000000);
        }


        glGenBuffers(1, &_offsetsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _offsetsBuffer);
        //extremely important to mark this as a dynamic draw buffer
        glBufferData(GL_ARRAY_BUFFER,  MGE_MAX_PARTICLES_PER_SYSTEM*_vec3Size, &dummy_vec3[0], GL_DYNAMIC_DRAW);



        glGenBuffers(1, &_lifeScaleBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _lifeScaleBuffer);
        //extremely important to mark this as a dynamic draw buffer
        glBufferData(GL_ARRAY_BUFFER, MGE_MAX_PARTICLES_PER_SYSTEM*_vec2Size, &dummy_vec2[0], GL_DYNAMIC_DRAW);

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

    int particleLimit = pSys->_particleLimit;

    //void* target;

    glBindBuffer(GL_ARRAY_BUFFER, _offsetsBuffer);
///   mapBuffer method - get a pointer from OpenGL to write raw memory to with memcpy
//    target = glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
//    memcpy(target,&(pSys->_positionsMain[0]),particleLimit*sizeof(glm::vec3));
//    glUnmapBuffer( GL_ARRAY_BUFFER );

///   bufferData method - discards current buffer memory and allocates new contents
//    glBufferData(GL_ARRAY_BUFFER,particleLimit*sizeof(glm::vec3),&(pSys->_positionsMain[0]),GL_STATIC_DRAW);

///   bufferSubData method - modify existing buffer memory
    glBufferSubData(GL_ARRAY_BUFFER,0,particleLimit*_vec3Size, &(pSys->_positionsMain[0]));
    glEnableVertexAttribArray(_posLoc);
    glVertexAttribPointer(_posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );


    glBindBuffer(GL_ARRAY_BUFFER, _lifeScaleBuffer);
    glBufferSubData(GL_ARRAY_BUFFER,0,particleLimit*_vec2Size, &(pSys->_lifeScaleMain[0]));
    glEnableVertexAttribArray(_lifeScaleLoc);
    glVertexAttribPointer(_lifeScaleLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );


    //THE draw call
    glDrawArrays(GL_POINTS,0,particleLimit);
    Renderer::debugInfo.drawCallCount++;
    //Renderer::debugInfo.triangleCount += particleLimit *



    glDisableVertexAttribArray(_posLoc);
    glDisableVertexAttribArray(_lifeScaleLoc);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}
