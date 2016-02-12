#include <iostream>
using namespace std;

#include <glm.hpp>
#include "mge/core/Camera.hpp"

Camera::Camera( std::string pName, glm::vec3 pPosition, glm::mat4 pProjectionMatrix )
:	GameObject(pName, pPosition)//, _projection(pProjectionMatrix)
{
    _projection = glm::perspective (glm::radians(60.0f), -4.0f/3.0f, 0.1f, 1000.0f  );
}

Camera::~Camera()
{
	//dtor
}

glm::mat4& Camera::getProjection() {
    return _projection;
}

