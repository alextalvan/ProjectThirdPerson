#include <mge/behaviours/FirstPersonLook.hpp>
#include <mge/util/Input.hpp>
#include <mge/collision/CollisionManager.hpp>
#include "SFML/Window.hpp"

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
    //newTransform[2] = glm::vec4(0,0,1,0);//debug

    newTransform *= glm::rotate( glm::radians(-rotationX), glm::vec3(0.0f, 1.0f, 0.0f));
    newTransform *= glm::rotate( glm::radians(-rotationY), glm::vec3(1.0f, 0.0f, 0.0f));

    _owner->setWorldTransform(newTransform);

    /*
    //temp testing raycast
    glm::vec3 origin = glm::vec3(newTransform[3]);
    glm::vec3 dir = glm::normalize(glm::vec3(newTransform[2])) * -1.0f;


    RaycastInfo res;
    Ray ray(origin,dir);
    if(Input::GetKey(Input::J) &&CollisionManager::Raycast(ray,res))
    {
        std::cout<<res.object->getName()<<"\n";
    }
    else
        std::cout<<"\n";
    */

}

glm::vec2 FirstPersonLook::GetMouseOffset()
{
	glm::vec2 mousePos = glm::vec2(Input::getMouseX(), Input::getMouseY());

	glm::vec2 offset = mousePos - prevMousePos;

	prevMousePos= mousePos;

	return offset;
}
