#include "CharacterController.hpp"
#include "mge/core/GameObject.hpp"

CharacterController::CharacterController(float moveSpeed, float turnSpeed)
: KeysBehaviour(moveSpeed, turnSpeed)
{
    _name = "Character Controller";
}

void CharacterController::Update()
{
    Movement();
    Rotation();
}

void CharacterController::Movement()
{
	float moveX = 0.0f;
	float moveZ = 0.0f;

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

	//translate the object in its own local space
	glm::mat4 temp = _owner->getWorldTransform();

	glm::vec3 forward = -glm::vec3(temp[2]);
	glm::vec3 right = -glm::vec3(temp[0]);
    glm::vec3 forward1 = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
    glm::vec3 right1 = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

    temp[0] = temp[1] = temp[2] = glm::vec4(0);
	temp[3] += glm::vec4(right1 * moveX,0);
	temp[3] += glm::vec4(forward1 * moveZ,0);
	_owner->setWorldTransform(temp);
}

void CharacterController::Rotation()
{
    glm::vec2 mouseOffset = GetMouseOffset();

    rotationX += mouseOffset.x;
    rotationY += mouseOffset.y;

    if (rotationY > 89.0f)
		rotationY = 89.0f;
	if (rotationY < -89.0f)
		rotationY = -89.0f;



    glm::mat4 newTransform;

    newTransform = glm::translate(newTransform,_owner->getWorldPosition());
    newTransform *= glm::rotate( -rotationX/150.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    newTransform *= glm::rotate( -rotationY/150.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    _owner->setTransform(newTransform);

    //_owner->translate(pos);
	//rotate the object in its own local space
	//_owner->rotate( glm::radians(rotationX * _turnSpeed), glm::vec3(0.0f, 1.0f, 0.0f ) );
	//_owner->rotate( glm::radians(rotationY * _turnSpeed), glm::vec3(1.0f, 0.0f, 0.0f ) );
}

glm::vec2 CharacterController::GetMouseOffset()
{
	glm::vec2 mousePos = glm::vec2(Input::getMouseX(), Input::getMouseY());

	//Non zero check
	//if (mousePos.x - prevMousePos.x == 0 && mousePos.y - prevMousePos.y == 0)
	//	return glm::vec2(0, 0);

	glm::vec2 offset = mousePos - prevMousePos;
	prevMousePos = mousePos;

	//std::cout << "X: " + std::to_string(offset.x) + "; " + "Y: " + std::to_string(offset.y) << std::endl;

	return offset;
}

CharacterController::~CharacterController()
{
    //dtor
}
