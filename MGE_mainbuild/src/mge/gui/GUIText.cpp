#include <GL/glew.h>
#include <SFML/Graphics/Text.hpp>
#include "mge/gui/GUIText.hpp"

using namespace std;

GUIText::GUIText( sf::Font& pFont, std::string pText, float pPosX, float pPosY, float pRotation, int pTextSize, sf::Color pColor)
:   GUI("GUIText", pPosX, pPosY), _text(pText, pFont, pTextSize), _textColor(pColor)
{
    sf::FloatRect textRect = _text.getLocalBounds();
    _text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
    setTextPosition(pPosX, pPosY);
    setTextRotation(pRotation);
	setTextColor(_textColor);
}

GUIText::~GUIText()
{
	//dtor
}

void GUIText::setOrigin(float x, float y)
{
    sf::FloatRect textRect = _text.getLocalBounds();
    _text.setOrigin(textRect.left + x, textRect.top + y);
}

void GUIText::setTextString(std::string pText) {
    _text.setString(pText);
}

void GUIText::setTextPosition(float pPosX, float pPosY) {
	setLocalPosition(glm::vec3(pPosX, pPosY, 0.0f));
	_text.setPosition(getWorldPosition().x, getWorldPosition().y);
}

void GUIText::setTextRotation(float pAngle)
{
    glm::mat4 rotMat = glm::rotate(glm::radians(pAngle),glm::vec3(0, 0, 1));
	setLocalRotation(glm::vec3(rotMat[0]),glm::vec3(rotMat[1]),glm::vec3(rotMat[2]));
    glm::vec2 xDir(getWorldTransform()[0].x,getWorldTransform()[0].y);
    float rot = atan2(xDir.y,xDir.x);
    _text.setRotation(glm::degrees(rot));
}

void GUIText::setTextFont(sf::Font& pFont)
{
    _text.setFont(pFont);
}

void GUIText::setTextSize(int pTextSize)
{
    _text.setCharacterSize(pTextSize);
}

void GUIText::setTextColor(sf::Color pColor)
{
	_text.setColor(pColor);
}

void GUIText::Refresh2DTransform()
{
    glm::mat4 worldTr = getWorldTransform();

    glm::vec2 pos = glm::vec2(worldTr[3].x,worldTr[3].y);
    glm::vec2 scale = glm::vec2(glm::length(worldTr[0]),glm::length(worldTr[1]));
    float rot = atan2(worldTr[0].y,worldTr[0].x);

    _text.setPosition(pos.x,pos.y);
    _text.setRotation(rot);
    _text.setScale(scale.x,scale.y);
}

void GUIText::fade()
{
    if (_fade) {
        sf::Color col = _text.getColor();
        if (_fadeState) {
            if (col.a > _fadeSpeed)
                _text.setColor(sf::Color(col.r,col.g,col.b,col.a - sf::Uint8(_fadeSpeed)));
            else
                _text.setColor(sf::Color(col.r,col.g,col.b,sf::Uint8(0)));
        } else {
            if (col.a + _fadeSpeed < 255)
                _text.setColor(sf::Color(col.r,col.g,col.b,col.a + sf::Uint8(_fadeSpeed)));
            else
                _text.setColor(sf::Color(col.r,col.g,col.b,sf::Uint8(255)));
        }

        col = _text.getColor();
        if (col.a <= 0 || col.a >= 255) {
            _fade = false;
            return;
        }
    }
}

void GUIText::InnerDraw(sf::RenderTarget& target)
{
	//glDisable( GL_CULL_FACE );
    target.draw(_text);
}
