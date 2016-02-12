#ifndef MGEDEMO_H
#define MGEDEMO_H

#include <mge/core/AbstractGame.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include "mge/lua/LuaScript.hpp"
class Swapper;
class GameObject;
class DebugHud;
class GUIText;
class GUISprite;
class Resources;

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
	    Resources* resources;
		DebugHud* _hud;                   //hud display
		GUIText* guiText;
		GUISprite* guiSprite;
        LuaScript* luaScript;
        void _updateHud();
};

#endif // MGEDEMO_H
