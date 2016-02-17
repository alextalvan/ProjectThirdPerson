#pragma once
#include "mge/behaviours/Component.hpp"
#include "mge/core/Mesh.hpp"
#include "glm.hpp"
//#include "mge/ut

#define MGE_MAX_PARTICLES_PER_SYSTEM 1024
#define MGE_NEW_PARTICLE_BUFFER_SIZE 128

namespace particles
{
    struct Particle
    {
        float lifeTime = -1.0f;
        glm::vec3 position;
        glm::vec3 speed;
    };
}

using namespace particles;
class ParticleSystem : public Component
{


public:
    ParticleSystem();
    static void Initialize();

    //start position
    //float startMinX = 0, startMaxX = 0, startMinY = 0, startMaxY = 0, startMinZ = 0, startMaxZ = 0;

    //variable speed
    float speedMinX = 0, speedMaxX = 0, speedMinY = 0, speedMaxY = 0, speedMinZ = 0, speedMaxZ = 0;

    //lifetime range
    float minLifetime = 5, maxLifetime = 5;

    //float


protected:
    virtual ~ParticleSystem();
    void Update();
    virtual void Release();

private:
    Particle _particles[MGE_MAX_PARTICLES_PER_SYSTEM];
    Particle _buffer[MGE_NEW_PARTICLE_BUFFER_SIZE];
    int _bufferCount = 0;

    static Mesh* _particleQuad;
};


