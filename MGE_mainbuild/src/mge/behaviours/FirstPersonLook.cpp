#include <mge/behaviours/FirstPersonLook.hpp>
#include <mge/util/Input.hpp>

FirstPersonLook::FirstPersonLook(float turnSpeed)
{
    _name = "FirstPersonVision";
    _turnSpeed = turnSpeed;
}

FirstPersonLook::~FirstPersonLook()
{

}

void FirstPersonLook::Update()
{
    Rotation();
}

void FirstPersonLook::Rotation()
{
    glm::vec2 mouseOffset = GetMouseOffset();

    if(abs(mouseOffset.x) > 200) mouseOffset.x = 0;
    if(abs(mouseOffset.y) > 200) mouseOffset.y = 0;

    rotationX -= mouseOffset.x / 5;
    rotationY += mouseOffset.y / 5;

    if (rotationY > 89.0f)
		rotationY = 89.0f;
	if (rotationY < -89.0f)
		rotationY = -89.0f;



    glm::mat4 newTransform;


    newTransform = glm::translate(newTransform,_owner->getWorldPosition());
    newTransform *= glm::rotate( glm::radians(-rotationX), glm::vec3(0.0f, 1.0f, 0.0f));
    newTransform *= glm::rotate( glm::radians(-rotationY), glm::vec3(1.0f, 0.0f, 0.0f));
    _owner->setWorldTransform(newTransform);

    //std::cout<<"test";

    //_owner->translate(pos);
	//rotate the object in its own local space
	//_owner->rotate( glm::radians(rotationX * _turnSpeed), glm::vec3(0.0f, 1.0f, 0.0f ) );
	//_owner->rotate( glm::radians(rotationY * _turnSpeed), glm::vec3(1.0f, 0.0f, 0.0f ) );
}

glm::vec2 FirstPersonLook::GetMouseOffset()
{
	glm::vec2 mousePos = glm::vec2(Input::getMouseX(), Input::getMouseY());

	glm::vec2 offset = mousePos - prevMousePos;
	prevMousePos = mousePos;

	return offset;
}
