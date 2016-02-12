#pragma once

#include "mge/gui/GUI.hpp"

class GUIText : public GUI
{
	public:
		GUIText( sf::RenderWindow * pWindow, sf::Font& pFont, float pPosX = 0.0f, float pPosY = 0.0f, float pRotation = 0.0f,
                 std::string pText = "Text", int pTextSize = 10, sf::Color pColor = sf::Color::White );
		void setTextFont (sf::Font& pFont);
		void setTextPosition (float pPosX, float pPosY);
		void setTextRotation(float pAngle);
		void setTextString (std::string text);
		void setTextSize (int pTextSize);
		void setTextColor (sf::Color pColor);
	protected:
	    virtual ~GUIText();
		virtual void drawCurrent(GUI& target) ;
    private:
        sf::Text _text;
        sf::Color _textColor;
        void initializeSettings();
};
