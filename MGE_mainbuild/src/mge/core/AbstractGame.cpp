#include "AbstractGame.hpp"

#include <iostream>
//using namespace std;

#include "mge/core/Time.hpp"
#include "mge/core/FPS.hpp"
#include "mge/core/Renderer.hpp"
#include "mge/core/World.hpp"
#include <mge/collision/CollisionManager.hpp>
#include <mge/particles/ParticleSystem.hpp>
#include <mge/util/Input.hpp>
#include <mge/util/Random.hpp>
#include <mge/gui/GUI.hpp>
#include <mge/lua/LuaScript.hpp>
#include <mge/config.hpp>
#include <fstream>


//debugging by stepping into individual frames
//bool testBool = false;

sf::RenderWindow* AbstractGame::_window = 0;

AbstractGame::AbstractGame():_renderer(NULL),_world(NULL),_world2D(NULL),_running(false)
{
    //initialize game systems

}
sf::RenderWindow* const AbstractGame::GetWindow()
{
    return _window;
}


AbstractGame::~AbstractGame()
{
    //dtor
    delete _window;
    delete _renderer;
    delete _world;
}

void AbstractGame::initialize() {
    std::cout << "Initializing engine...\n\n";
    std::cout << "Reading config.txt..\n";

    ifstream f(config::MGE_ROOT_PATH + "config.txt");
    std::string s;
    int width, height,fullscreen;
    f>>s; f>>width;
    f>>s; f>>height;
    f>>s; f>>fullscreen;
    std::cout << "config loaded.."<<width<<" "<<height<<" "<<fullscreen<<std::endl;//\n";

    f.close();


    _initializeWindow(width,height,fullscreen);
    _printVersionInfo();
    _initializeGlew();
    _initializeRenderer(width,height);
    _initializeWorld();
    _initializeScene();

    CollisionManager::Initialize();
    std::cout << std::endl << "Engine initialized." << std::endl << std::endl;
}

///SETUP

void AbstractGame::_initializeWindow(int width, int height, int fullscreen)
{
	std::cout << "Initializing window..." << std::endl;

	int style = (fullscreen) ? sf::Style::Fullscreen : sf::Style::Titlebar;
	_window = new sf::RenderWindow( sf::VideoMode(width,height), "FairWind Game Engine", style , sf::ContextSettings(24,8,4,3,3));
	_window->setMouseCursorVisible(false);
	_window->setFramerateLimit(0);
	//_window->setVerticalSyncEnabled(true);
    std::cout << "Window initialized." << std::endl << std::endl;
}

void AbstractGame::_printVersionInfo() {
	cout << "Context info:" << endl;
    cout << "----------------------------------" << endl;
    //print some debug stats for whoever cares
    const GLubyte *renderer = glGetString( GL_RENDERER );
    const GLubyte *vendor = glGetString( GL_VENDOR );
    const GLubyte *version = glGetString( GL_VERSION );
    const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );
    //nice consistency here in the way OpenGl retrieves values
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    printf("GL Vendor : %s\n", vendor);
    printf("GL Renderer : %s\n", renderer);
    printf("GL Version (string) : %s\n", version);
    printf("GL Version (integer) : %d.%d\n", major, minor);
    printf("GLSL Version : %s\n", glslVersion);

    cout << "----------------------------------" << endl << endl;
}

void AbstractGame::_initializeGlew() {
	std::cout << "Initializing GLEW..." << std::endl;
    //initialize the opengl extension wrangler
    GLint glewStatus = glewInit();
	std::cout << "Initialized GLEW, status (1 == OK, 0 == FAILED):" << (glewStatus == GLEW_OK) << std::endl << std::endl;
}

void AbstractGame::_initializeRenderer(int width, int height)
{
    //setup our own renderer
	std::cout << "Initializing renderer..." << std::endl;
	_renderer = new Renderer(width,height);
    std::cout << "Renderer done." << std::endl << std::endl;
}

void AbstractGame::_initializeWorld() {
    //setup our own renderer
	std::cout << "Initializing world..." << std::endl;
	_world = new World(_window);
	_world2D = new GUI();

    std::cout << "Attaching main lua script..." << std::endl;
	_world->AttachComponent(new LuaScript("main.lua",_world, _world2D));
	//_world
    std::cout << "World initialized." << std::endl << std::endl;
}

///LOOP

void AbstractGame::run()
{
	_running = true;
	_timeAccumultor = 0.0f;

	_renderer->setClearColor(49,77,121);

	while (_running)
    {
		//this is used internally for calling update routines and throttling FPS (when enabled)
		Time::gameLoopUpdate();

		//this is the time used by everything else. Time::update() is always ok to call to force a timestamp refresh
		//(for example when counting how many seconds one function takes)
		Time::update();


        TimedStep();
	}
}

void AbstractGame::TimedStep()
{
    _timeAccumultor+= Time::gameLoopDelta();
    bool logicUpdated = false;

    while(_timeAccumultor>= _targetFrameTime)
    {
        _processEvents();
        CollisionManager::DoCollisions();
        _update();
        logicUpdated = true;
        _timeAccumultor -= _targetFrameTime;
        Input::EndLoopReset();
    }

    //use the catchup mechanism for game logic but not for rendering, rendering multiple times is useless
    if(logicUpdated)
    {
        Destroyable::CollectGarbage();

//        FPS::update();
//        _render();
//        _window->display();
    }
        //have this here for unlocked framerate
        _render();
        _window->display();
        FPS::update();

}

void AbstractGame::_update() {
    _world->InternalUpdate();
    _world2D->InternalUpdate();
}

void AbstractGame::_render ()
{
    //render 3d world
    _renderer->render(_world);

    //gui overlay
    _window->pushGLStates();
    _world2D->DrawTo(*_window);
    _window->popGLStates();
}

void AbstractGame::_processEvents()
{
	Input::PollInputEvents(_window);

    if(!_window->isOpen())
    {
        _running = false;
        return;
    }

    //exit key
	if(Input::GetKeyDown(Input::Escape))
    {
         _window->close();
         _running = false;
    }
}



