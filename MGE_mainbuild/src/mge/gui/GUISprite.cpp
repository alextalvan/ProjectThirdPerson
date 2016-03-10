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
    setSpriteRotation(pRotation);
	setSpritePosition (pPosX, pPosY);
	setSpriteScale(pScaleX, pScaleY);
}

GUISprite::~GUISprite()
{
	//dtor
}

void GUISprite::setOrigin(float x, float y)
{
    sf::FloatRect spriteRect = _sprite.getLocalBounds();
    _sprite.setOrigin(spriteRect.left + x, spriteRect.top + y);
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
	setLocalRotation(glm::vec3(rotMat[0]),glm::vec3(rotMat[1]),glm::vec3(rotMat[2]));
    glm::vec2 xDir(getWorldTransform()[0].x,getWorldTransform()[0].y);
    float rot = atan2(xDir.y,xDir.x);
    _sprite.setRotation(glm::degrees(rot));
}

void GUISprite::setSpriteScale(float pScaleX, float pScaleY) {
    scale(glm::vec3(pScaleX, pScaleY, 0.0f));

    //glm::vec4 xVector = getWorldTransform()[0];
    //glm::vec4 yVector = getWorldTransform()[1];

    _sprite.setScale(pScaleX, pScaleY);//glm::length(xVector),glm::length(yVector));
}

void GUISprite::fade()
{
    if (_fade) {
        sf::Color col = _sprite.getColor();
        if (_fadeState) {
            if (col.a > _fadeSpeed)
                _sprite.setColor(sf::Color(col.r,col.g,col.b,col.a - sf::Uint8(_fadeSpeed)));
            else
                _sprite.setColor(sf::Color(col.r,col.g,col.b,sf::Uint8(0)));
        } else {
            if (col.a + _fadeSpeed < 255)
                _sprite.setColor(sf::Color(col.r,col.g,col.b,col.a + sf::Uint8(_fadeSpeed)));
            else
                _sprite.setColor(sf::Color(col.r,col.g,col.b,sf::Uint8(255)));
        }

        col = _sprite.getColor();
        if (col.a <= 0 || col.a >= 255) {
            _fade = false;
        }
    }
}

void GUISprite::InnerDraw(sf::RenderTarget& target)
{
	//glDisable( GL_CULL_FACE );
    target.draw(_sprite);
}

void GUISprite::Refresh2DTransform()
{
    glm::mat4 worldTr = getWorldTransform();

    glm::vec2 pos = glm::vec2(worldTr[3].x,worldTr[3].y);
    glm::vec2 scale = glm::vec2(glm::length(worldTr[0]),glm::length(worldTr[1]));
    float rot = atan2(worldTr[0].y,worldTr[0].x);

    _sprite.setPosition(pos.x,pos.y);
    _sprite.setRotation(rot);
    _sprite.setScale(scale.x,scale.y);
}
