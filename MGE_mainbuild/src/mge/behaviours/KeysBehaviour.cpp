#include "mge/behaviours/KeysBehaviour.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/util/Input.hpp"

#include <sfml/window/event.hpp>

KeysBehaviour::KeysBehaviour(float moveSpeed, float turnSpeed): Component(), _moveSpeed(moveSpeed), _turnSpeed(turnSpeed)
{
    _name = "Movement Behaviour";
}

KeysBehaviour::~KeysBehaviour()
{
}

void KeysBehaviour::Update()
{
	float moveSpeed = 0.0f; //default if no keys
	float turnSpeed = 0.0f;
	if ( Input::GetKey(Input::Up)) {
		moveSpeed = _moveSpeed;
	}
	if ( Input::GetKey(Input::Down)) {
		moveSpeed = -_moveSpeed;
	}
	if ( Input::GetKey(Input::Right)) {
		turnSpeed = -_turnSpeed;
	}
	if ( Input::GetKey(Input::Left)) {
		turnSpeed = +_turnSpeed;
	}
	//translate the object in its own local space
	_owner->translate( glm::vec3(0.0f, 0.0f, moveSpeed ) );

	//rotate the object in its own local space
	_owner->rotate( glm::radians(turnSpeed), glm::vec3(0.0f, 1.0f, 0.0f ) );

	//if ( Input::GetKey(Input::Space)) {
	//	_owner->Destroy();
	//}
}
