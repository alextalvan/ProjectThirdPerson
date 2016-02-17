#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include "mge/gui/GUISprite.hpp"
#include "SFML/Graphics/Transform.hpp"

using namespace std;

GUISprite::GUISprite(sf::Texture& pTexture, float pPosX, float pPosY, float pRotation, float pScaleX, float pScaleY)
:   GUI("GUISprite", pPosX, pPosY), _sprite(pTexture)
{
    sf::FloatRect spriteRect = _sprite.getLocalBounds();
    _sprite.setOrigin(spriteRect.left + spriteRect.width/2.0f, spriteRect.top  + spriteRect.height/2.0f);
	setSpritePosition (pPosX, pPosY);
	setSpriteRotation(pRotation);
	setSpriteScale(pScaleX, pScaleY);
}

GUISprite::~GUISprite()
{
	//dtor
}

void GUISprite::setSpriteTexture(sf::Texture& pTexture) {
    _sprite.setTexture(pTexture);
}

void GUISprite::setSpritePosition(float pPosX, float pPosY) {
	setLocalPosition(glm::vec3(pPosX, pPosY, 0.0f));
	_sprite.setPosition(getWorldPosition().x, getWorldPosition().y);
}


void GUISprite::setSpriteRotation(float pAngle)
{
    glm::mat4 rotMat = glm::rotate(glm::radians(pAngle),glm::vec3(0, 0, 1));
	setLocalRotation(rotMat[0],rotMat[1],rotMat[2]);
    glm::vec2 xDir(getWorldTransform()[0].x,getWorldTransform()[0].y);
    float rot = atan2(xDir.y,xDir.x);
    _sprite.setRotation(glm::degrees(rot));
}

void GUISprite::setSpriteScale(float pScaleX, float pScaleY) {
    scale(glm::vec3(pScaleX, pScaleY, 0.0f));

    glm::vec4 xVector = getWorldTransform()[0];
    glm::vec4 yVector = getWorldTransform()[1];

    _sprite.setScale(glm::length(xVector),glm::length(yVector));
}

void GUISprite::InnerDraw(sf::RenderTarget& target)
{
	//glDisable( GL_CULL_FACE );
	glActiveTexture(GL_TEXTURE0);
    target.pushGLStates();
    target.draw(_sprite);
	target.popGLStates();
}
