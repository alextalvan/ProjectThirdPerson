#pragma once

#include "mge/gui/GUI.hpp"

class GUIText : public GUI
{
	public:
		GUIText(sf::Font& pFont, std::string pText = "Text", float pPosX = 0.0f, float pPosY = 0.0f,
        float pRotation = 0.0f, int pTextSize = 10, sf::Color pColor = sf::Color::White);
		void setTextFont (sf::Font& pFont);
		void setTextPosition (float pPosX, float pPosY);
		void setTextRotation(float pAngle);
		void setTextString (std::string text);
		void setTextSize (int pTextSize);
		void setTextColor (sf::Color pColor);
        void Refresh2DTransform() override;
		void setOrigin(float x, float y);
		void setOrigin();
	protected:
	    virtual ~GUIText();
	    void InnerDraw(sf::RenderTarget& target) override;
		virtual void fade();
    private:
        sf::Text _text;
        sf::Color _textColor;
        void initializeSettings();
};
