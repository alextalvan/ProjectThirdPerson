#include <mge/behaviours/CameraOrbit.hpp>
#include <mge/util/Input.hpp>
#include <mge/core/Camera.hpp>
#include <iostream>

namespace Test
{
    CameraOrbit::CameraOrbit()
    {
        prevMouseX = Input::getMouseX();
        prevMouseY = Input::getMouseY();
    }

    CameraOrbit::~CameraOrbit()
    {

    }

    void CameraOrbit::Update()
    {
        if(!dynamic_cast<Camera*>(_owner) || target == NULL)
            return;

        int mouseX = Input::getMouseX();

        if(mouseX!=prevMouseX && abs(mouseX - prevMouseX) < 100)
        {
            currentHorizAngle += (mouseX - prevMouseX) * horizRotationSpeed;
        }

        prevMouseX = mouseX;



        int mouseY = Input::getMouseY();

        if(mouseY!=prevMouseY && abs(mouseY - prevMouseY) < 100)
        {
            currentVertAngle -= (mouseY - prevMouseY) * vertRotationSpeed;

            currentVertAngle = glm::clamp(currentVertAngle,-maxVertAngle,maxVertAngle);
        }

        _owner->setTransform(glm::mat4(1));
        _owner->rotate(glm::radians(currentHorizAngle),glm::vec3(0,1,0));
        _owner->translate(glm::vec3(0,verticalDistance,horizontalDistance));

        glm::mat4 ownerMat = _owner->getTransform();
        glm::vec3 forwardVec = glm::normalize(_owner->getLocalPosition());
        ownerMat[2] = glm::vec4(forwardVec,0);
        glm::vec3 rightVec = glm::normalize(glm::cross(glm::vec3(0,1,0), forwardVec));
        ownerMat[0] = glm::vec4(rightVec,0);
        ownerMat[1] = glm::vec4(glm::normalize(glm::cross(forwardVec,rightVec)),0);
        _owner->setTransform(ownerMat);
        _owner->rotate(glm::radians(currentVertAngle),glm::vec3(1,0,0));

        prevMouseY = mouseY;

        //std::cout<<currentVertAngle<<endl;
    }
}
