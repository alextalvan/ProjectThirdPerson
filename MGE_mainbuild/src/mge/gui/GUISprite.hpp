#pragma once

#include "mge/gui/GUI.hpp"

class GUISprite : public GUI
{
	public:
		GUISprite( sf::RenderWindow * pWindow, sf::Texture& pTexture, float pPosX = 0.0f,
            float pPosY = 0.0f, float pRotation = 0.0f, float _scaleX = 1.0f, float _scaleY = 1.0f);
		void setSpriteTexture (sf::Texture& pTexture);
		void setSpritePosition (float pPosX, float pPosY);
		void setSpriteRotation(float pAngle);
		void setSpriteScale (float pScaleX, float ScaleY);
	protected:
	    virtual ~GUISprite();
		virtual void drawCurrent(GUI& target) ;
    private:
        sf::Sprite _sprite;
        void initializeSettings();
};
