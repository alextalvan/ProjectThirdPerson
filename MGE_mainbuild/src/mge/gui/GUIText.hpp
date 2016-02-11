#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "mge/core/GameObject.hpp"

class GUIText : public GameObject, public sf::Drawable
{
	public:
		GUIText( sf::RenderWindow * aWindow, /*sf::Font& pFont, */glm::vec2 pPosition = glm::vec2(0,0),
                 std::string pText = "Text", int pTextSize = 10, sf::Color pColor = sf::Color::White );
		virtual ~GUIText();
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		void setTextFont (sf::Font& pFont);
		void setTextPosition (glm::vec2 pPosition);
		void setTextString (std::string text);
		void setTextSize (int pTextSize);
		void setTextColor (sf::Color pColor);
	private:
		sf::RenderWindow * _window;
        string _textStr;
        int _textSize;
        sf::Color _textColor;
        sf::Font _font;
        sf::Text _text;

        void initializeSettings();
};
