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

//construct the game class into _window, _renderer and hud (other parts are initialized by build)
MGEDemo::MGEDemo():AbstractGame ()
{
}

void MGEDemo::initialize() {
    //setup the core part
    AbstractGame::initialize();

    //setup the custom part
	cout << "Initializing HUD" << endl;
	_hud = new DebugHud(_window);
	cout << "HUD initialized." << endl << endl;
}

void CreateWall(glm::vec3 pos, float horizRot, AbstractMaterial* mat, Mesh* mesh, World* world)
{
    GameObject* sphere2 = new GameObject("wall", pos);
    sphere2->setMesh(mesh);
    sphere2->setMaterial(mat);
    sphere2->rotate(horizRot,glm::vec3(0,1,0));

    WallCollider* wall = new WallCollider();
    sphere2->AttachComponent(wall);

    world->AddChild(sphere2);
}

//build the game _world
void MGEDemo::_initializeScene()
{
    _renderer->setClearColor(0,0,0);

    //MESHES

    //load a bunch of meshes we will be using throughout this demo
    //each mesh only has to be loaded once, but can be used multiple times:
    //F is flat shaded, S is smooth shaded (normals aligned or not), check the models folder!
    Mesh* planeMeshDefault = Mesh::load (config::MGE_MODEL_PATH+"plane_8192.obj");
    Mesh* raptorMesh = Mesh::load(config::MGE_MODEL_PATH+"raptor.obj");
    //Mesh* cubeMeshF = Mesh::load (config::MGE_MODEL_PATH+"cube_flat.obj");
    //Mesh* suzannaMeshF = Mesh::load (config::MGE_MODEL_PATH+"suzanna_flat.obj");
    Mesh* sphereMeshF = Mesh::load(config::MGE_MODEL_PATH+"sphere_smooth.obj");
    //Mesh* teapotMeshS = Mesh::load (config::MGE_MODEL_PATH+"teapot_smooth.obj");

    //MATERIALS

    AbstractMaterial* colorMaterial = new ColorMaterial (glm::vec3(0.2f,0,0.2f));
    AbstractMaterial* terrainMaterial = new TerrainMaterial (Texture::load (config::MGE_TEXTURE_PATH+"heightmap_custom.png"),
                                                             Texture::load (config::MGE_TEXTURE_PATH+"splatmap_custom.png"),
                                                             Texture::load (config::MGE_TEXTURE_PATH+"diffuse1.jpg"),
                                                             Texture::load (config::MGE_TEXTURE_PATH+"diffuse3.jpg"),
                                                             Texture::load (config::MGE_TEXTURE_PATH+"diffuse4.jpg"),
                                                             Texture::load (config::MGE_TEXTURE_PATH+"water.jpg"));

    AbstractMaterial* textureMaterial2 = new TextureLitMaterial (Texture::load (config::MGE_TEXTURE_PATH+"bricks.jpg"));
    AbstractMaterial* textureMaterial3 = new TextureLitMaterial (Texture::load (config::MGE_TEXTURE_PATH+"raptor.png"));

    //SCENE SETUP

    Camera* camera = new Camera ("camera", glm::vec3(0,5,0));
    camera->rotate(glm::radians(-90.0f),glm::vec3(1,0,0));
    _world->setMainCamera(camera);
    //_world->AddChild(camera);

    GameObject* plane = new GameObject ("plane");
    plane->scale(glm::vec3(20,5,20));
    plane->setMesh(planeMeshDefault);
    plane->setMaterial(textureMaterial2);
    _world->AddChild(plane);


    GameObject* raptor = new GameObject ("raptor",glm::vec3(5,0,0));
    raptor->setMesh(raptorMesh);
    raptor->setMaterial(textureMaterial3);
    raptor->scale(glm::vec3(0.02f,0.02f,0.02f));
    //raptor->rotate(glm::radians(90.0f),glm::vec3(0,1,0));
    //raptor->AttachComponent(new KeysBehaviour(1,1));
    _world->AddChild(raptor);
    /*
    GameObject* raptorContainer = new GameObject("raptor root");
    raptorContainer->AttachComponent(new KeysBehaviour());
    raptorContainer->AddChild(raptor);
    _world->AddChild(raptorContainer);

    Light* light2 = new Light();
    light2->color = glm::vec3(1,1,1);
    light2->attenuation = glm::vec3(0.5f,0,0.75f);
    light2->type = MGE_LIGHT_POINT;
    */

    Light* light1 = new Light();
    light1->color = glm::vec3(1,1,1);
    light1->direction = glm::vec3(1,0,0);
    light1->type = MGE_LIGHT_DIRECTIONAL;



    //light3::setWorldTransform(glm::)

    //light3->

    GameObject* sphere1 = new GameObject ("object_controlled", glm::vec3(0,0,0));
    sphere1->setMesh (sphereMeshF);
    sphere1->setMaterial(textureMaterial2);
    sphere1->AttachComponent(new KeysBehaviour(0.05f,1.0f));
    sphere1->AttachComponent(new SphereCollider());
    //sphere1->AttachComponent(new Test::TestSphereCol());
    _world->AddChild(sphere1);

    Light* light3 = new Light();
    light3->color = glm::vec3(1,0,0);
    //light3->attenuation = glm::vec3(0.75f,0,0.5f);
    light3->attenuation = glm::vec3(1.0f,0.010f,0.0005f);
    light3->type = MGE_LIGHT_SPOTLIGHT;
    //light3->rotate(glm::radians(-90.0f),glm::vec3(1,0,0));
    //light3->translate(glm::vec3(0,10000,0));
    //light3->angle = 3.14f;
    light3->setLocalPosition(glm::vec3(0,2,0));
    sphere1->AddChild(light3);

    //sphere1->AddChild(camera);


    //CreateWall(glm::vec3(0,0.0f,0),0,colorMaterial,cubeMeshF,_world);



    //Sound::SoundManager::GetSingleton()->LoadSFX(config::MGE_SOUND_PATH + "test.wav")->Play();

    luaScript = new LuaScript((config::MGE_SCRIPT_PATH + "test.lua").c_str(), _world);
    //_world->AttachComponent(luaScript);
    sphere1->AttachComponent(luaScript);

    //sphere1->Destroy();
}





void MGEDemo::_render() {
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
