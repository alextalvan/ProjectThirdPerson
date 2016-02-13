#pragma once

#include <SFML/Graphics.hpp>
#include "mge/core/GameObject.hpp"

class GUI : public GameObject
{
	public:
        GUI(sf::RenderWindow * pWindow, std::string pName = "GUI", float pPosX = 0.0f, float pPosY = 0.0f);
		virtual void draw(GUI& target);
    protected:
        virtual ~GUI();
		sf::RenderWindow * _window;
		virtual void drawCurrent(GUI& target);
};
