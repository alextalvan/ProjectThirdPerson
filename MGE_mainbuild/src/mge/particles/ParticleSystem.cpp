#include "mge/particles/ParticleSystem.hpp"
#include "mge/config.hpp"
#include <iostream>


Mesh* ParticleSystem::_particleQuad;// = Mesh::load("particleQuad");

void ParticleSystem::Initialize()
{
    std::cout<<"Initializing particle systems...\n";
    _particleQuad = Mesh::load(config::MGE_MODEL_PATH + "particleQuad.obj");
}

ParticleSystem::ParticleSystem()
{

}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::Update()
{



    //_bufferCount = 0;
}
