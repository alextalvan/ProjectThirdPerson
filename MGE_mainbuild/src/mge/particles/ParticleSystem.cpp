#include "mge/particles/ParticleSystem.hpp"
#include "mge/config.hpp"
#include "mge/util/Random.hpp"
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

ParticleSystem::ParticleSystem()
{
    setMesh(Mesh::load(config::MGE_MODEL_PATH + "particleQuad.obj"));//this is useless for actual rendering but I do it so the recursive render method lets it draw (if mesh!=NULL && material!=NULL)
}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::Update()
{

    if(_releaseTimer.isFinished())
    {
        _releaseTimer.SetDuration(Utils::Random::GetFloatValue(minReleaseDelay,maxReleaseDelay));
        _releaseTimer.Reset();
        _cachedWorldPos = getWorldPosition();

        Release();
    }

    _bufferCount = 0;
}

void ParticleSystem::Release()
{
    int amount = Utils::Random::GetValue(minParticlesPerRelease,maxParticlesPerRelease);

    if(amount > MGE_NEW_PARTICLE_BUFFER_SIZE)
    {
        amount = MGE_NEW_PARTICLE_BUFFER_SIZE;
        std::cout<<"Particle buffer overflow! You are spawning too many particles per frame. max: "<<MGE_NEW_PARTICLE_BUFFER_SIZE<<"\n";
    }

    for(int i=0;i<amount;++i)
    {
        _buffer[i].position = _cachedWorldPos;
        _buffer[i].lifeTime = Utils::Random::GetFloatValue(minLifetime,maxLifetime);
        _buffer[i].speed.x = Utils::Random::GetFloatValue(speedMinX,speedMaxX);
        _buffer[i].speed.y = Utils::Random::GetFloatValue(speedMinY,speedMaxY);
        _buffer[i].speed.z = Utils::Random::GetFloatValue(speedMinZ,speedMaxZ);
    }
}
