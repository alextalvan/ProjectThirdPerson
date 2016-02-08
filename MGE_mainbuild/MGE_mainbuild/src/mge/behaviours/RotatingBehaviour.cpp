#include "mge/behaviours/RotatingBehaviour.hpp"
#include "mge/core/GameObject.hpp"

RotatingBehaviour::RotatingBehaviour():Component()
{
	//ctor
}

RotatingBehaviour::~RotatingBehaviour()
{
	//dtor
}

void RotatingBehaviour::Update()
{
	_owner->rotate(glm::radians(1.0f), glm::vec3( 1.0f, 1.0f, 0.0f ) ); // rotates 1° per step
}
