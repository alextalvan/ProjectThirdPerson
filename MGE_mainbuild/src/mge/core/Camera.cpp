#include <iostream>
using namespace std;

#include <glm.hpp>
#include "mge/core/Camera.hpp"
#include<mge/behaviours/Component.hpp>
#include<mge/core/GameObject.hpp>
#include<mge/behaviours/FirstPersonLook.hpp>

Camera::Camera( std::string pName, glm::vec3 pPosition, glm::mat4 pProjectionMatrix )
:	GameObject(pName, pPosition)//, _projection(pProjectionMatrix)
{
    _projection = glm::perspective (glm::radians(60.0f), -16.0f/9.0f, 0.1f, 1000.0f  );

    FirstPersonLook* vis = new FirstPersonLook();
    AttachComponent(vis);
}

Camera::~Camera()
{
	//dtor
}

glm::mat4& Camera::getProjection()
{
    return _projection;
}

