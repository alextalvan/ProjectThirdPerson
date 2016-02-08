#include <cassert>
#include <algorithm>
#include <iostream>
#include "mge/core/World.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"

using namespace std;

World::World():GameObject("root"), _mainCamera(0)
{
	//ctor
	_transform = glm::mat4();
	_worldTransform = glm::mat4();
	GameObject::_world = this;
}

void World::setMainCamera (Camera* pCamera) {
    if (pCamera != NULL) _mainCamera = pCamera;
}

Camera* World::getMainCamera () {
    return _mainCamera;
}

void World::renderDebugInfo() {
    //walk through all meshes and debug their normals
    GameObject* gameObject;
    for (int i = 0; i < GetChildCount(); i++) {
        gameObject = GetChildAt(i);
        //oo wise it would be better to do gameObject->renderDebugInfo, but since this is only for debugging
        //it's sort of pasted on
        if (gameObject->getMaterial() && gameObject->getMesh()) {
            gameObject->getMesh()->renderDebugInfo(gameObject->getWorldTransform(), this);
        }
    }
}

