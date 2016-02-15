#include "AbstractGame.hpp"

#include <iostream>
using namespace std;

#include "mge/core/Time.hpp"
#include "mge/core/FPS.hpp"
#include "mge/core/Renderer.hpp"
#include "mge/core/World.hpp"
#include <mge/collision/CollisionManager.hpp>
#include <mge/util/Input.hpp>
#include <mge/util/Random.hpp>


//debugging by stepping into individual frames
//bool testBool = false;

AbstractGame::AbstractGame():_window(NULL),_renderer(NULL),_world(NULL),_running(false)
{
    CollisionManager::Initialize();
    Random::Initialize();
}

AbstractGame::~AbstractGame()
{
    //dtor
    delete _window;
    delete _renderer;
    delete _world;
}

void AbstractGame::initialize() {
    cout << "Initializing engine..." << endl << endl;
    _initializeWindow();
    _printVersionInfo();
    _initializeGlew();
    _initializeRenderer();
    _initializeWorld();
    _initializeScene();
    cout << endl << "Engine initialized." << endl << endl;
}

///SETUP

void AbstractGame::_initializeWindow() {
	cout << "Initializing window..." << endl;
	_window = new sf::RenderWindow( sf::VideoMode(1366,768), "FairWind Game Engine", sf::Style::Titlebar, sf::ContextSettings(24,8,4,3,3));
	_window->setVerticalSyncEnabled(true);
    cout << "Window initialized." << endl << endl;
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
	cout << "Initializing GLEW..." << endl;
    //initialize the opengl extension wrangler
    GLint glewStatus = glewInit();
	cout << "Initialized GLEW, status (1 == OK, 0 == FAILED):" << (glewStatus == GLEW_OK) << endl << endl;
}

void AbstractGame::_initializeRenderer() {
    //setup our own renderer
	cout << "Initializing renderer..." << endl;
	_renderer = new Renderer(1366,768);
    cout << "Renderer done." << endl << endl;
}

void AbstractGame::_initializeWorld() {
    //setup our own renderer
	cout << "Initializing world..." << endl;
	_world = new World(_window);
	//_world
    cout << "World initialized." << endl << endl;
}

///LOOP

void AbstractGame::run()
{
	_running = true;
	_timeAccumultor = 0.0f;

	_renderer->setClearColor(49,77,121);

	while (_running)
    {
        //debug
        /*
        std::cout<<"newframe\n";

        if(testBool)
        {
            if(getchar() == 'a')
                testBool = !testBool;
        }
        if(Input::GetKeyUp(Input::A))
            testBool = !testBool;

        */
        //enddebug

        //_processEvents();

		Time::update();

        TimedStep();
		//cout<<"Game frame time: "<<Time::gameLoopDelta()<<" Render frame time: "<<Time::renderDelta()<<'\n';
	}
}

void AbstractGame::TimedStep()
{
    _timeAccumultor+= Time::gameLoopDelta();
    bool logicUpdated = false;

    while(_timeAccumultor>= _targetFrameTime)
    {
        _processEvents();
        _update();
        CollisionManager::DoCollisions();
        logicUpdated = true;
        _timeAccumultor -= _targetFrameTime;
        Input::EndLoopReset();
    }

    //use the catchup mechanism for game logic but not for rendering, rendering multiple times is useless
    if(logicUpdated)
    {
        Destroyable::CollectGarbage();

        Time::updateRenderTime();
        FPS::update();
        _render();
        _window->display();
    }

}

void AbstractGame::_update() {
    _world->InternalUpdate();
}

void AbstractGame::_render ()
{
    _renderer->render(_world);
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



