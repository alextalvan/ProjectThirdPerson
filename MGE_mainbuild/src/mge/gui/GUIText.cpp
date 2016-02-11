#include <cassert>
#include <iostream>
using namespace std;

#include "mge/gui/GUIText.hpp"
#include <cstdio>
#include <GL/glew.h>
#include <SFML/Graphics/Text.hpp>
#include "mge/core/FPS.hpp"

#include "mge/config.hpp"

GUIText::GUIText( sf::RenderWindow * aWindow, /*sf::Font& pFont,*/ glm::vec2 pPosition,
                 std::string pText, int pTextSize, sf::Color pColor)
:   GameObject("GUIText", glm::vec3(pPosition, 0.0f)), _window( aWindow ), /*_font(pFont), */_textStr(pText), _textSize(pTextSize), _textColor(pColor)
{
	assert ( _window != NULL );

    if (!_font.loadFromFile(config::MGE_FONT_PATH+ "arial.ttf")) {
        cout << "Could not load font, exiting..." << endl;
        return;
    }

    sf::FloatRect textRect = _text.getLocalBounds();
    _text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);

    initializeSettings();
}

GUIText::~GUIText()
{
	//dtor
}

void GUIText::initializeSettings()
{
    _text.setString(_textStr);
    _text.setPosition(getLocalPosition().x,getLocalPosition().y);
    _text.setFont(_font);
	_text.setCharacterSize(_textSize);
	_text.setColor(_textColor);
}

void GUIText::setTextString(std::string pText) {
    _text.setString(pText);
}

void GUIText::setTextPosition(glm::vec2 pPosition) {
	_text.setPosition(pPosition.x, pPosition.y);
	setLocalPosition(glm::vec3(pPosition, 0.0f));
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

void GUIText::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//glDisable( GL_CULL_FACE );
	//glActiveTexture(GL_TEXTURE0);
    _window->pushGLStates();
    _window->draw(_text);
	_window->popGLStates();
}
