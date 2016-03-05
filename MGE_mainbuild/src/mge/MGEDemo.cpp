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
#include <mge/collision/BoxCollider.hpp>
#include <mge/collision/CollisionManager.hpp>

#include <mge/sound/SoundManager.hpp>
#include <mge/sound/SoundChannel.hpp>

#include <mge/editor/LevelImporter.hpp>

#include "mge/gui/GUI.hpp"
#include "mge/gui/GUIText.hpp"
#include "mge/gui/GUISprite.hpp"
#include "mge/util/ResourceCacher.hpp"

#include "mge/particles/ParticleSystem.hpp"

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
    //return;
    Camera* cam = new Camera("cam", glm::vec3(15,15,15));
    _world->setMainCamera(cam);
    _world->AddChild(cam);


    Mesh* planeMesh = Mesh::load(config::MGE_MODEL_PATH + "plane20x20_2tris_aligned_uvs.obj");
    Mesh* cubeMesh = Mesh::load(config::MGE_MODEL_PATH + "cube_flat.obj");
    Texture* brickTex = Texture::load(config::MGE_TEXTURE_PATH + "brickwall.jpg");
    Texture* brickNorm = Texture::load(config::MGE_TEXTURE_PATH + "brickwall_normal.jpg");
    Texture* brickMet = Texture::load(config::MGE_TEXTURE_PATH + "brickwall.jpg");
    TextureLitMaterial* litMat = new TextureLitMaterial(brickTex, 0.2f, 32.0f, 0.1f, brickMet, brickNorm);

    GameObject* plane = new GameObject("plane", glm::vec3(0,-1,0));
    plane->setMesh(planeMesh);
    plane->setMaterial(litMat);
    plane->scale(glm::vec3(-10,1,10));
    _world->AddChild(plane);

    GameObject* testCube = new GameObject("cube", glm::vec3(0,0,20));
    testCube->setMesh(cubeMesh);
    testCube->setMaterial(litMat);
    testCube->scale(glm::vec3(-1,1,1));
    //testCube->scale(glm::vec3(0.5,0.5,0.5));
    _world->AddChild(testCube);

    GameObject* testCube1 = new GameObject("cube1", glm::vec3(3,0,3));
    testCube1->setMesh(cubeMesh);
    testCube1->setMaterial(litMat);
    testCube1->rotate(0.46, glm::vec3(0,1,0));
    testCube1->scale(glm::vec3(-1,1,1));
    _world->AddChild(testCube1);

    testCube1 = new GameObject("cube1", glm::vec3(3,0,3));
    testCube1->setMesh(cubeMesh);
    testCube1->setMaterial(litMat);
    testCube1->rotate(0.46, glm::vec3(0,1,0));
    testCube1->scale(glm::vec3(-1,1,1));
    _world->AddChild(testCube1);

    testCube1 = new GameObject("cube1", glm::vec3(3,0,3));
    testCube1->setMesh(cubeMesh);
    testCube1->setMaterial(litMat);
    testCube1->rotate(0.46, glm::vec3(0,1,0));
    testCube1->scale(glm::vec3(-1,1,1));
    _world->AddChild(testCube1);

    testCube1 = new GameObject("cube1", glm::vec3(3,0,3));
    testCube1->setMesh(cubeMesh);
    testCube1->setMaterial(litMat);
    testCube1->rotate(0.46, glm::vec3(0,1,0));
    testCube1->scale(glm::vec3(-1,1,1));
    _world->AddChild(testCube1);

    testCube1 = new GameObject("cube1", glm::vec3(3,0,3));
    testCube1->setMesh(cubeMesh);
    testCube1->setMaterial(litMat);
    testCube1->rotate(0.46, glm::vec3(0,1,0));
    testCube1->scale(glm::vec3(-1,1,1));
    _world->AddChild(testCube1);

    testCube1 = new GameObject("cube1", glm::vec3(7,0,3));
    testCube1->setMesh(cubeMesh);
    testCube1->setMaterial(litMat);
    testCube1->rotate(0.46, glm::vec3(0,1,0));
    testCube1->scale(glm::vec3(-1,1,1));
    _world->AddChild(testCube1);

    testCube1 = new GameObject("cube1", glm::vec3(10,0,-5));
    testCube1->setMesh(cubeMesh);
    testCube1->setMaterial(litMat);
    testCube1->rotate(0.46, glm::vec3(0,1,0));
    testCube1->scale(glm::vec3(-1,1,1));
    _world->AddChild(testCube1);

    testCube1 = new GameObject("cube1", glm::vec3(3,0,3));
    testCube1->setMesh(cubeMesh);
    testCube1->setMaterial(litMat);
    //testCube1->rotate(0.46, glm::vec3(0,1,0));
    testCube1->scale(glm::vec3(-1,1,1));


    testCube1 = new GameObject("cube1", glm::vec3(0,0,0));
    testCube1->setMesh(cubeMesh);
    testCube1->setMaterial(litMat);
    //testCube1->rotate(0.46, glm::vec3(0,1,0));
    //testCube1->scale(glm::vec3(-1,1,1));



    Light* light1 = new Light(MGE_LIGHT_DIRECTIONAL, glm::vec3(-15,15,-15), glm::vec3(1, -3, 1), glm::vec3(1.0f,1.0f,1.0f));
    //_world->AddChild(light1);
    //light1->AddChild(testCube1);
    //testCube1->setLocalPosition(glm::vec3(0));
    //light1->setLocalPosition(glm::vec3(-30,30,-30));
    //testCube->AddChild(light1);
    //Light* light2 = new Light(MGE_LIGHT_POINT, glm::vec3(0,0.5,2), glm::vec3(1,1,1), glm::vec3(1,0,0), glm::vec3(0.1f,0.1f,0.1f));
    //Light* light3 = new Light(MGE_LIGHT_SPOTLIGHT, glm::vec3(0,2,2), glm::vec3(0,-1,0), glm::vec3(0,0,1), glm::vec3(0.1f,0.1f,0.1f), 0.36f);
    cam->AttachComponent(new LookAt(testCube));
    testCube->AttachComponent(new KeysBehaviour(0.1,1));

    //Texture* water = Texture::load(config::MGE_TEXTURE_PATH + "smoke.png");

//    ParticleSystem* part = new ParticleSystem(water);
//    //part->minLifetime = part->maxLifetime = 3.0f;
//    part->lifeTimeRange = glm::vec2(3);
//    part->releaseCount = glm::vec2(128);
//    part->releaseDelay = glm::vec2(0.5f,1.0f);
//    part->speedMin = glm::vec3(-1.0f,0.01f,-1.0f);
//    part->speedMax = glm::vec3(1.0f,0.02f,1.0f);
//    //part->startOffset1 = glm::vec3(-1);
//    //part->startOffset2 = glm::vec3(1);
//    part->scaleRange1 = part->scaleRange2 = glm::vec2(2.0f);
//
//    part->setWorldPosition(glm::vec3(0,5,0));
//    _world->AddChild(part);

}

void MGEDemo::_render()
{
    AbstractGame::_render();
    _updateHud();
    //_world->renderDebugInfo();
}

void MGEDemo::_updateHud() {
    string debugInfo = "";
    debugInfo += string ("FPS:") + std::to_string(FPS::getFPS()) +
    "\nDraw calls: " + std::to_string(Renderer::debugInfo.drawCallCount)+
    "\nTriangles: " + std::to_string(Renderer::debugInfo.triangleCount)+
    "\nRender time(ms): " + std::to_string(Renderer::debugInfo.time)+
    "\nCollision tests: " + std::to_string(CollisionManager::debugInfo.tests)+
    "\nCollision time(ms): " + std::to_string(CollisionManager::debugInfo.time);

    _hud->setDebugInfo(debugInfo);
    _hud->draw();
}


MGEDemo::~MGEDemo()
{
	//dtor
}
