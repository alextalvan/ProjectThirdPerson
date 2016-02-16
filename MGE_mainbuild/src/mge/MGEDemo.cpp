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
#include "mge/util/ResourceCacher.hpp"

//construct the game class into _window, _renderer and hud (other parts are initialized by build)
MGEDemo::MGEDemo():AbstractGame ()
{

}

void MGEDemo::initialize() {
    //setup the core part
    AbstractGame::initialize();
    //setup the custom part
	_hud = new DebugHud(_window);
}

//build the game _world
void MGEDemo::_initializeScene()
{
    Camera* camera = new Camera("camera", glm::vec3(3, 3, 3));
    _world->setMainCamera(camera);
    _world->AddChild(camera);

    Mesh* cubeMesh = Mesh::load(config::MGE_MODEL_PATH + "cube_flat.obj");
    TextureLitMaterial* cubeMat = new TextureLitMaterial(Texture::load(config::MGE_TEXTURE_PATH + "brickwall.jpg"), Texture::load(config::MGE_TEXTURE_PATH + "brickwall_normal.jpg"));
    GameObject * testCube = new GameObject("cube", glm::vec3(0,0,0));
    testCube->setMesh(cubeMesh);
    testCube->setMaterial(cubeMat);
    _world->AddChild(testCube);

    Light* light1 = new Light();
    light1->color = glm::vec3(1,1,1);
    light1->direction = glm::vec3(1,1,1);
    light1->type = MGE_LIGHT_DIRECTIONAL;
    camera->AttachComponent(new LookAt(testCube));
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

void MGEDemo::updateGUI()
{
    GUI * gui = _world->getGui();
    gui->draw(*gui);
}

MGEDemo::~MGEDemo()
{
	//dtor
}
