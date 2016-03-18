#include "mge/particles/ParticleSystem.hpp"
#include "mge/config.hpp"
#include "mge/util/Random.hpp"
#include "mge/core/Time.hpp"

#include "mge/materials/Particles/ParticleMaterial.hpp"
#include <iostream>


//Mesh* ParticleSystem::_particleQuad;// = Mesh::load("particleQuad");

/*
void ParticleSystem::Initialize()
{
    std::cout<<"Initializing particle systems...\n";
    _particleQuad = Mesh::load(config::MGE_MODEL_PATH + "particleQuad.obj");
    std::cout<<"Particle systems initialized\n";
}
*/

ParticleSystem::ParticleSystem(Texture* particleTex)
{
    setMesh(Mesh::load(config::MGE_MODEL_PATH + "particleQuad.obj"));//this is useless for actual rendering but I do it so the recursive render method lets it draw (if mesh!=NULL && material!=NULL)
    _lastTime = Time::now();
    setMaterial(new ParticleMaterial(particleTex));
    _releaseTimer.SetDuration(0.0f);
    _releaseTimer.Reset();
    scaleRange = glm::vec2(1);
    castShadows = false;

    SetTransparent(true);

    for(int i=0;i<MGE_MAX_PARTICLES_PER_SYSTEM;++i)
    {
        _lifeTimesMain[i] = -1.0f;
        _scalesMain[i] = 1.0f;
        _positionsMain[i] = glm::vec3(-1000000);
    }
}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::Update()
{
    UpdateParticles();
    if(_emitterEnabled && _releaseTimer.isFinished())
    {
        _releaseTimer.SetDuration(Utils::Random::GetFloatValue(releaseDelay.x,releaseDelay.y));
        _releaseTimer.Reset();
        _cachedWorldPos = getWorldPosition();

        Release();
    }

    //_bufferCount = 0;
}

void ParticleSystem::Release()
{
    int amount = Utils::Random::GetValue(releaseCount.x,releaseCount.y);

    if(amount > MGE_NEW_PARTICLE_BUFFER_SIZE)
    {
        amount = MGE_NEW_PARTICLE_BUFFER_SIZE;
        std::cout<<"Particle buffer overflow! You are spawning too many particles per frame. max: "<<MGE_NEW_PARTICLE_BUFFER_SIZE<<"\n";
    }

    for(int i=0;i<amount;++i)
    {
        glm::vec3 randomOffset;
        randomOffset.x = Utils::Random::GetFloatValue(startOffset1.x,startOffset2.x);
        randomOffset.y = Utils::Random::GetFloatValue(startOffset1.y,startOffset2.y);
        randomOffset.z = Utils::Random::GetFloatValue(startOffset1.z,startOffset2.z);

        _scalesBuffer[i] = Utils::Random::GetFloatValue(scaleRange.x,scaleRange.y);


        _positionsBuffer[i] = _cachedWorldPos + randomOffset;
        _lifeTimesBuffer[i] = Utils::Random::GetFloatValue(lifeTimeRange.x,lifeTimeRange.y);

        _speedsBuffer[i].x = Utils::Random::GetFloatValue(speedMin.x,speedMax.x);
        _speedsBuffer[i].y = Utils::Random::GetFloatValue(speedMin.y,speedMax.y);
        _speedsBuffer[i].z = Utils::Random::GetFloatValue(speedMin.z,speedMax.z);
    }

    _bufferCount = amount;
    //std::cout<<"realease\n";
}

void ParticleSystem::UpdateParticles()
{
    //Time::update();
    float delta = Time::now() - _lastTime;
    _lastTime = Time::now();
    for(int i=0;i<MGE_MAX_PARTICLES_PER_SYSTEM;++i)
    {
        _lifeTimesMain[i] -= delta;

        if(_lifeTimesMain[i].x>0.0f)
        {
            _positionsMain[i] += _speedsMain[i];
        }
    }
}

void ParticleSystem::ToggleEmitter(bool val)
{
//    if(val)
//    {
//        _lastTime = Time::now();
//        _releaseTimer.SetDuration(0.0f);
//        _releaseTimer.Reset();
//    }

    _emitterEnabled = val;
}

void ParticleSystem::SetParticleLimit(int amount)
{
    if(amount > MGE_MAX_PARTICLES_PER_SYSTEM)
        amount = MGE_MAX_PARTICLES_PER_SYSTEM;

    if(amount < 0)
        amount = 0;

    _particleLimit = amount;
}

//int ParticleSystem::GetParticleLimit()
//{
//    return _particleLimit;
//}
