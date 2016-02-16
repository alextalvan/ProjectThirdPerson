#ifndef MGEDEMO_H
#define MGEDEMO_H

#include <mge/core/AbstractGame.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include "mge/lua/LuaScript.hpp"

class Swapper;
class GameObject;
class DebugHud;
class GUI;

class MGEDemo: public AbstractGame
{
    //PUBLIC FUNCTIONS

	public:
		MGEDemo();
		virtual ~MGEDemo();

        virtual void initialize();

	protected:
        virtual void _initializeScene();

	    //override render to render the hud as well.
	    virtual void _render();

	private:
		DebugHud* _hud;                   //hud display
		GUI* gui;
        LuaScript* luaScript;
        void _updateHud();
        void updateGUI();
};

#endif // MGEDEMO_H
