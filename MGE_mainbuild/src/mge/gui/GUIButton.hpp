#pragma once

#include "mge/gui/GUISprite.hpp"

class GUIButton : public GUISprite
{
	public:
		GUIButton( sf::RenderWindow * pWindow, sf::Texture& pTexture1, sf::Texture& pTexture2,
            float pPosX = 0.0f, float pPosY = 0.0f, float pScaleX = 1.0f, float pScaleY = 1.0f, float pRotation = 0.0f);
        virtual void setSpriteTexture (sf::Texture& pTexture, int index);
		bool onClick();
	protected:
	    virtual ~GUIButton();
		virtual void drawCurrent(GUI& target);
    private:
        sf::Texture & _active;
        sf::Texture & _inactive;
        bool pressed = false;
        void initializeSettings();
        bool onHover();
};
