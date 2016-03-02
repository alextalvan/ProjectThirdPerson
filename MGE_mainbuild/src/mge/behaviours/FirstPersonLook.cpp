#include <mge/behaviours/FirstPersonLook.hpp>
#include <mge/util/Input.hpp>
#include <mge/collision/CollisionManager.hpp>
#include <mge/util/list/DualLinkList.hpp>
#include <mge/util/list/DualLinkNode.hpp>
#include <mge/collision/Collider.hpp>
#include <mge/lua/LuaScript.hpp>
#include "SFML/Window.hpp"
#include <mge/core/Renderer.hpp>
#include<mge/collision/QuadTree.hpp>

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


    float reach = 10.0f;

    glm::vec3 origin = glm::vec3(newTransform[3]);
    glm::vec3 dir = glm::normalize(glm::vec3(newTransform[2])) * -1.0f;


    RaycastInfo res;
    Ray ray(origin,dir);
    if(Input::GetKeyDown(Input::E) && CollisionManager::Raycast(ray,res) && res.distance <= reach)
    {
        DualLinkNode<Component>* cn = res.object->GetComponents().startNode;
        while(cn!=NULL)
        {
            LuaScript* script = dynamic_cast<LuaScript*>((Component*)cn);
            if(script!=NULL && script->IsActive())
                script->InvokeFunction("OnInteract");
            cn = cn->nextNode;
        }
        //std::cout<<"\n"<<res.object->GetComponent<Collider>()->quadTreeOwner->_depth;
        //std::cout<<"distance: "<<res.distance<<"\n";
    }

}

glm::vec2 FirstPersonLook::GetMouseOffset()
{
	glm::vec2 mousePos = glm::vec2(Input::getMouseX(), Input::getMouseY());

	glm::vec2 offset = mousePos - prevMousePos;

    float newX = mousePos.x;
    glm::vec2 screenSize = Renderer::GetScreenSize();

	if(mousePos.x > screenSize.x * 0.75f || mousePos.x < screenSize.x * 0.25f) newX = screenSize.x * 0.5f;

	float newY = mousePos.y;
	if(mousePos.y > screenSize.y * 0.75f || mousePos.y < screenSize.y * 0.25f) newY = screenSize.y * 0.5f;

    sf::Mouse::setPosition(sf::Vector2i((int)newX,(int)newY),*AbstractGame::GetWindow());

    mousePos.x = newX;
    mousePos.y = newY;

	prevMousePos = mousePos;

	return offset;
}
