#pragma once

#include <SFML/Graphics.hpp>
#include "mge/core/GameObject.hpp"

class GUI : public GameObject
{
	public:
        GUI(std::string pName = "GUI Element", float pPosX = 0.0f, float pPosY = 0.0f);
		//virtual void draw(GUI& target);
		void DrawTo(sf::RenderTarget& target);
    protected:
        virtual ~GUI();
        virtual void InnerDraw(sf::RenderTarget& target);
    private:

		//sf::RenderWindow * _window;
		//virtual void drawCurrent(GUI& target);
};
