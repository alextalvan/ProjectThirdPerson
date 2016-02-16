#include <cassert>
#include <algorithm>
#include <iostream>
#include "mge/core/World.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/lua/LuaScript.hpp"
#include "mge/config.hpp"
#include "mge/gui/GUI.hpp"

using namespace std;

Camera* World::_mainCamera = NULL;

World::World(sf::RenderWindow * pWindow):GameObject("root")
{
    gui = new GUI(pWindow);
	//ctor
	_transform = glm::mat4();
	_worldTransform = glm::mat4();
	//GameObject::_world = this;
	AttachComponent(new LuaScript((config::MGE_SCRIPT_PATH + "main.lua").c_str(), this, gui));
}

void World::setMainCamera (Camera* pCamera) {
    if (pCamera != NULL)
    {
        _mainCamera = pCamera;
    }
}

Camera* World::getMainCamera () {
    return _mainCamera;
}

GUI* World::getGui() {
    return gui;
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


