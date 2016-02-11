#pragma once

#include <SFML/Graphics.hpp>
#include "mge/core/GameObject.hpp"

class GUIText : public GameObject, public sf::Drawable
{
	public:
		GUIText( sf::RenderWindow * pWindow, sf::Font& pFont, glm::vec2 pPosition = glm::vec2(0,0),
                 std::string pText = "Text", int pTextSize = 10, sf::Color pColor = sf::Color::White );
		~GUIText();
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		void setTextFont (sf::Font& pFont);
		void setTextPosition (glm::vec2 pPosition);
		void setTextString (std::string text);
		void setTextSize (int pTextSize);
		void setTextColor (sf::Color pColor);
    private:
		sf::RenderWindow * _window;
        sf::Color _textColor;
        sf::Text _text;
        void initializeSettings();
};
