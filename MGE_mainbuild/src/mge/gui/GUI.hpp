#pragma once

#include <SFML/Graphics.hpp>
#include "mge/core/GameObject.hpp"

class GUI : public GameObject
{
	public:
        GUI(std::string pName = "GUI Element", float pPosX = 0.0f, float pPosY = 0.0f);
		//virtual void draw(GUI& target);
		void DrawTo(sf::RenderTarget& target);
		void SetGUIActive(bool active);
        virtual void Refresh2DTransform();
    protected:
        virtual ~GUI();
        virtual void InnerDraw(sf::RenderTarget& target);

    private:
        bool _active = true;
		//sf::RenderWindow * _window;
		//virtual void drawCurrent(GUI& target);
};
