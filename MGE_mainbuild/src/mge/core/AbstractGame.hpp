#ifndef ABSTRACTGAME_H
#define ABSTRACTGAME_H

#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <string>

using namespace std;

class World;
class Renderer;
class GUI;

/**
 * Defines the basic structure for a game, with defaults already filled in.
 * Your main game loop should run:
 *
 *  initialize
 *  run
 *
 * in order.
 */
class AbstractGame
{
    public:

        AbstractGame();
        virtual ~AbstractGame();

        //creates a window, initializes glew, a renderer and a world instance
        virtual void initialize();
        //run the actual process of updating all objects, rendering them and processing events
        virtual void run();

        static sf::RenderWindow* const GetWindow();

    protected:

        //methods above delegate behaviour to the methods below so that you can override it in a subclass

        //initialize sfml rendering context
        virtual void _initializeWindow(int width, int height, int fullscreen);
        //print info about the current driver version etc
        virtual void _printVersionInfo();
        //initialize the extension wrangler
        virtual void _initializeGlew();
        //create our own custom renderer instance
        virtual void _initializeRenderer(int width, int height);
        //initialize a scene root to which we can attach/add objects
        virtual void _initializeWorld();

        //initialize the actual scene, to be done by a subclass
        virtual void _initializeScene() = 0;

        //call update on all game objects in the display root
        virtual void _update();
        //render all game objects in the display root
        virtual void _render();
        //process any sfml window events (see SystemEventDispatcher/Listener)
        virtual void _processEvents();

		static sf::RenderWindow* _window;  //sfml window to render into
		Renderer* _renderer;        //the renderer class to render the world
		World* _world;              //the root game object that represents our scene
		GUI* _world2D;
		bool _running;

    private:
        float _timeAccumultor = 0.0f;
        float _targetFrameTime = 0.016666f;

        void TimedStep();

};

#endif // ABSTRACTGAME_H
