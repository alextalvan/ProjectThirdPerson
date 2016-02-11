#pragma once

#include <SFML/Graphics.hpp>
#include "mge/core/GameObject.hpp"

class GUISprite : public GameObject, public sf::Drawable
{
	public:
		GUISprite( sf::RenderWindow * pWindow, sf::Texture& pTexture, glm::vec2 pPosition = glm::vec2(0,0), float _scaleX = 1.0f, float _scaleY = 1.0f);
		~GUISprite();
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		void setSpriteTexture (sf::Texture& pTexture);
		void setSpritePosition (glm::vec2 pPosition);
		void setSpriteScale (float pScaleX, float ScaleY);
    private:
		sf::RenderWindow * _window;
        sf::Sprite _sprite;
        void initializeSettings();
};
