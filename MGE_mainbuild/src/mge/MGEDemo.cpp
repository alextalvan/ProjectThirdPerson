#include <glm.hpp>
#include <iostream>
#include <string>
using namespace std;

#include "mge/core/Renderer.hpp"

#include "mge/core/Mesh.hpp"
#include "mge/core/World.hpp"
#include "mge/core/FPS.hpp"

#include "mge/core/Camera.hpp"

#include "mge/core/GameObject.hpp"
#include "mge/materials/AbstractMaterial.hpp"

#include "mge/materials/ColorMaterial.hpp"
#include "mge/materials/TextureMaterial.hpp"
#include "mge/materials/TerrainMaterial.hpp"
#include "mge/materials/TextureLitMaterial.hpp"
#include "mge/core/Light.hpp"

#include "mge/behaviours/RotatingBehaviour.hpp"
#include "mge/behaviours/KeysBehaviour.hpp"
#include "mge/behaviours/LookAt.hpp"

#include "mge/util/DebugHud.hpp"

#include "mge/config.hpp"
#include "mge/MGEDemo.hpp"

#include <mge/collision/SphereCollider.hpp>
#include <mge/test/TestSphereCol.hpp>
#include <mge/collision/WallCollider.hpp>
#include <mge/behaviours/CameraOrbit.hpp>

#include <mge/sound/SoundManager.hpp>
#include <mge/sound/SoundChannel.hpp>

#include <mge/editor/LevelImporter.hpp>

#include "mge/gui/GUI.hpp"
#include "mge/gui/GUIText.hpp"
#include "mge/gui/GUISprite.hpp"
#include "mge/util/ResourceCacher.hpp"

//construct the game class into _window, _renderer and hud (other parts are initialized by build)
MGEDemo::MGEDemo():AbstractGame ()
{

}

void MGEDemo::initialize() {
    //setup the core part
    AbstractGame::initialize();
    //setup the custom part
	gui = new GUI(_window);
	_hud = new DebugHud(_window);
}

//build the game _world
void MGEDemo::_initializeScene()
{
    /*
    GUISprite * sprite = new GUISprite(_window, *Utils::LoadTexture("bricks.jpg"), 50, 50);
    GUIText * text = new GUIText(_window, *Utils::LoadFont("Arial.ttf"), 200, 200);
    _world->AddChild(sprite);
    sprite->AddChild(text);
    */
}

void MGEDemo::_render() {
    AbstractGame::_render();

    ///no idea how to automatically draw all guis attached to world in other way so far. (i know this way is pretty ugly, but all other ways i can think of are as well)
    updateGUI();
    _updateHud();

    //_world->renderDebugInfo();
}

void MGEDemo::_updateHud() {
    string debugInfo = "";
    debugInfo += string ("FPS:") + std::to_string(FPS::getFPS())+"\n";

    _hud->setDebugInfo(debugInfo);
    _hud->draw();
}

void MGEDemo::updateGUI() {
    int childCount = _world->GetChildCount();
    if (childCount < 1) return;

    for (int i = 0; i < childCount; i++) {
         if (dynamic_cast<GUI*>(_world->GetChildAt(i)) != NULL)
         {
            GUI* gui = (GUI*)_world->GetChildAt(i);
            gui->draw(*gui);
         }
    }
}


MGEDemo::~MGEDemo()
{
	//dtor
}
