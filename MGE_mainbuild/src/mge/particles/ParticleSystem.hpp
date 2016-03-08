#pragma once
#include "mge/behaviours/Component.hpp"
#include "mge/core/Mesh.hpp"
#include "glm.hpp"
#include "mge/util/Timer.hpp"
#include "mge/core/Texture.hpp"
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
        glm::vec2 scale;
    };
}

class ParticleMaterial;

using namespace particles;
class ParticleSystem : public GameObject
{
friend class ParticleMaterial;
public:
    ParticleSystem(Texture* particleTex);
    //static void Initialize();
    //start position
    //float startMinX = 0, startMaxX = 0, startMinY = 0, startMaxY = 0, startMinZ = 0, startMaxZ = 0;
    glm::vec3 startOffset1, startOffset2;

    //variable speed
    //float speedMinX = 0, speedMaxX = 0, speedMinY = 0, speedMaxY = 0, speedMinZ = 0, speedMaxZ = 0;
    glm::vec3 speedMin, speedMax;

    //lifetime range
    //float minLifetime = 5, maxLifetime = 5;
    glm::vec2 lifeTimeRange;

    //delay between releases
    //float minReleaseDelay = 1, maxReleaseDelay = 1;
    glm::vec2 releaseDelay;

    //particles per "burst"
    //int minParticlesPerRelease = 1, maxParticlesPerRelease = 1;
    glm::vec2 releaseCount;

    glm::vec2 scaleRange;


protected:
    virtual ~ParticleSystem();
    void Update();
    virtual void Release();
    void UpdateParticles();

private:
    Particle _particles[MGE_MAX_PARTICLES_PER_SYSTEM];

    Particle _buffer[MGE_NEW_PARTICLE_BUFFER_SIZE];
    int _bufferCount = 0;

    Timer _releaseTimer;

    glm::vec3 _cachedWorldPos;

    float _lastTime;

    //static Mesh* _particleQuad;
};


