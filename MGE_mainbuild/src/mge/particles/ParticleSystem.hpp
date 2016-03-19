#pragma once
#include "mge/behaviours/Component.hpp"
#include "mge/core/Mesh.hpp"
#include "glm.hpp"
#include "mge/util/Timer.hpp"
#include "mge/core/Texture.hpp"
//#include "mge/ut

#define MGE_MAX_PARTICLES_PER_SYSTEM 1024
#define MGE_NEW_PARTICLE_BUFFER_SIZE 128



class ParticleMaterial;

//using namespace particles;
class ParticleSystem : public GameObject
{
friend class ParticleMaterial;
public:
    ParticleSystem(Texture* particleTex);

    glm::vec3 startOffset1, startOffset2;

    glm::vec3 speedMin, speedMax;

    glm::vec2 lifeTimeRange;

    glm::vec2 releaseDelay;

    glm::vec2 releaseCount;

    glm::vec2 scaleRange;

    void ToggleEmitter(bool val);

    void SetParticleLimit(int amount);
    //int GetParticleLimit();

protected:
    virtual ~ParticleSystem();
    void Update();
    virtual void Release();
    void UpdateParticles();

private:
    glm::vec3 _positionsMain[MGE_MAX_PARTICLES_PER_SYSTEM];
    glm::vec3 _speedsMain[MGE_MAX_PARTICLES_PER_SYSTEM];
    glm::vec2 _lifeScaleMain[MGE_MAX_PARTICLES_PER_SYSTEM];

    glm::vec3 _positionsBuffer[MGE_NEW_PARTICLE_BUFFER_SIZE];
    glm::vec3 _speedsBuffer[MGE_NEW_PARTICLE_BUFFER_SIZE];
    glm::vec2 _lifeScaleBuffer[MGE_NEW_PARTICLE_BUFFER_SIZE];

    int _bufferCount = 0;
    Timer _releaseTimer;
    glm::vec3 _cachedWorldPos;

    float _lastTime;

    bool _emitterEnabled = true;
    int _particleLimit = MGE_MAX_PARTICLES_PER_SYSTEM;

};


