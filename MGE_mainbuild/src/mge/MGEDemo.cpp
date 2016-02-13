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
	cout << "Initializing HUD" << endl;
	gui = new GUI(_window);
	_hud = new DebugHud(_window);
	guiSprite = new GUISprite(_window, *Utils::LoadTexture("osama.jpg"), _window->getSize().x/2, _window->getSize().y/2, 0.0f);
    gui->AddChild(guiSprite);

    //cout<<guiSprite->getWorldTransform();

	guiText = new GUIText(_window, *Utils::LoadFont("arial.ttf"), _window->getSize().x/2, _window->getSize().y/2, 0.0f, "ALLAHU AKBAR!", 50, sf::Color::Red);
	guiSprite->AddChild(guiText);

	cout << "Initializing HUD" << endl;
	_hud = new DebugHud(_window);
	cout << "HUD initialized." << endl << endl;
}

//build the game _world
void MGEDemo::_initializeScene()
{

}



void MGEDemo::_render() {
    AbstractGame::_render();
    gui->draw(*gui);
    _updateHud();

    //_world->renderDebugInfo();
}

void MGEDemo::_updateHud() {
    string debugInfo = "";
    debugInfo += string ("FPS:") + std::to_string(FPS::getFPS())+"\n";

    _hud->setDebugInfo(debugInfo);
    _hud->draw();
}


MGEDemo::~MGEDemo()
{
	//dtor
}
