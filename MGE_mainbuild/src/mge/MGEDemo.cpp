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
	_hud = new DebugHud(_window);
}

//build the game _world
void MGEDemo::_initializeScene()
{
    /*
    Camera* cam = new Camera("cam", glm::vec3(3,3,3));
    _world->setMainCamera(cam);
    _world->AddChild(cam);

    Mesh* cubeMesh = Mesh::load(config::MGE_MODEL_PATH + "cube_flat.obj");
    Texture* brickTex = Texture::load(config::MGE_TEXTURE_PATH + "brickwall.jpg");
    Texture* brickNorm = Texture::load(config::MGE_TEXTURE_PATH + "brickwall_normal.jpg");
    Texture* brickMet = Texture::load(config::MGE_TEXTURE_PATH + "brickwall.jpg");
    TextureLitMaterial* litMat = new TextureLitMaterial(brickTex, 0.25f, 32.0f, 0.1f, brickMet, brickNorm);

    //GameObject* planeCube = new GameObject("cube1", glm::vec3(0,-6,0));
    //planeCube->setMesh(cubeMesh);
    //planeCube->setMaterial(litMat);
    //planeCube->scale(glm::vec3(5,5,5));
    //_world->AddChild(planeCube);

    GameObject* testCube = new GameObject("cube2", glm::vec3(0,0,0));
    testCube->setMesh(cubeMesh);
    testCube->setMaterial(litMat);
    //testCube->scale(glm::vec3(0.5,0.5,0.5));
    _world->AddChild(testCube);


    Light* light1 = new Light(MGE_LIGHT_DIRECTIONAL, glm::vec3(50,50,50), testCube->getWorldPosition() - glm::vec3(5,3,3), glm::vec3(1,1,1));
    Light* light2 = new Light(MGE_LIGHT_POINT, glm::vec3(2,0,2), glm::vec3(1,1,1), glm::vec3(0,1,1), glm::vec3(0.1f,0.1f,0.1f));
    //Light* light3 = new Light(MGE_LIGHT_SPOTLIGHT, glm::vec3(0,2,2), glm::vec3(0,-1,0), glm::vec3(0,0,1), glm::vec3(0.1f,0.1f,0.1f), 0.36f);
    cam->AttachComponent(new LookAt(testCube));

    //GameObject* testCube2 = new GameObject("cube3", glm::vec3(0,0,2));
    //testCube2->setMesh(cubeMesh);
    //testCube2->setMaterial(litMat);
    //testCube2->scale(glm::vec3(0.15,0.15,0.15));
    //_world->AddChild(testCube2);
    */
}

void MGEDemo::_render()
{
    AbstractGame::_render();
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
