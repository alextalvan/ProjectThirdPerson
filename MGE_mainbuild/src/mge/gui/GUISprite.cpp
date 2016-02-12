#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include "mge/gui/GUISprite.hpp"

using namespace std;

GUISprite::GUISprite( sf::RenderWindow * pWindow, sf::Texture& pTexture, glm::vec2 pPosition, float pScaleX, float pScaleY)
:   GameObject("GUISprite", glm::vec3(pPosition, 0.0f)), _window( pWindow ), _sprite(pTexture)
{
	assert ( _window != NULL );

    sf::FloatRect spriteRect = _sprite.getLocalBounds();
    _sprite.setOrigin(spriteRect.left + spriteRect.width/2.0f, spriteRect.top  + spriteRect.height/2.0f);
	setSpritePosition (pPosition);
	setSpriteScale(pScaleX, pScaleY);
}

GUISprite::~GUISprite()
{
	//dtor
}

void GUISprite::setSpriteTexture(sf::Texture& pTexture) {
    _sprite.setTexture(pTexture);
}

void GUISprite::setSpritePosition(glm::vec2 pPosition) {
	_sprite.setPosition(pPosition.x, pPosition.y);
	setLocalPosition(glm::vec3(pPosition, 0.0f));
}

void GUISprite::setSpriteScale(float pScaleX, float pScaleY) {
    _sprite.setScale(pScaleX, pScaleY);
}

void GUISprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//glDisable( GL_CULL_FACE );
	//glActiveTexture(GL_TEXTURE0);
    _window->pushGLStates();
    _window->draw(_sprite);
	_window->popGLStates();
}
