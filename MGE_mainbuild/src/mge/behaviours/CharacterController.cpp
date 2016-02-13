#include "CharacterController.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/World.hpp"

CharacterController::CharacterController(float moveSpeed, float jumpStrength, float gravity)
{
    _name = "Character Controller";
    _moveSpeed = moveSpeed;
    _jumpForce = jumpStrength;
    _gravityForce = gravity;
}

void CharacterController::Update()
{
    Movement();
    //Rotation();
    //std::cout<<"test";
}

void CharacterController::Movement()
{
	float moveX = 0.0f;
	float moveZ = 0.0f;

	float jump = 0.0f;

	if ( Input::GetKey(Input::W)) {
		moveZ += _moveSpeed;
	}
	if ( Input::GetKey(Input::S)) {
		moveZ -= _moveSpeed;
	}
	if ( Input::GetKey(Input::D)) {
		moveX -= _moveSpeed;
	}
	if ( Input::GetKey(Input::A)) {
		moveX += _moveSpeed;
	}

	if ( Input::GetKey(Input::Space)) {
		jump += _jumpForce;
	}
    else
        jump -= _gravityForce;

	glm::mat4 temp = World::getMainCamera()->getWorldTransform();

	glm::vec3 forward = -glm::vec3(temp[2]);
	glm::vec3 right = glm::vec3(temp[0]);
    glm::vec3 forward1 = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
    glm::vec3 right1 = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

    glm::mat4 myMat = _owner->getWorldTransform();
	myMat[3] += glm::vec4(right1 * moveX,0);
	myMat[3] += glm::vec4(forward1 * moveZ,0);
	myMat[3] += glm::vec4(0,jump,0,0);
	_owner->setWorldTransform(myMat);
	//_owner->translate(glm::vec3(0,2,0));
}

CharacterController::~CharacterController()
{
    //dtor
}
