#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include "mge/gui/GUIButton.hpp"
#include "mge/util/Input.hpp"

using namespace std;

GUIButton::GUIButton(sf::Texture& pTexture1, sf::Texture& pTexture2, float pPosX, float pPosY,
            float pScaleX, float pScaleY, float pRotation)
            : GUISprite(pTexture2, pPosX, pPosY, pRotation, pScaleX, pScaleY), _active(pTexture1), _inactive(pTexture2)
{
    _sprite.setTexture(pTexture2);

    sf::FloatRect spriteRect = _sprite.getLocalBounds();
    _sprite.setOrigin(spriteRect.left + spriteRect.width/2.0f, spriteRect.top  + spriteRect.height/2.0f);
	setSpritePosition (pPosX, pPosY);
	setSpriteRotation(pRotation);
	setSpriteScale(pScaleX, pScaleY);
}

GUIButton::~GUIButton()
{
	//dtor
}

void GUIButton::setSpriteTexture(sf::Texture& pTexture, int index)  {
    if (index == 0)
        _active = pTexture;
    else
        _inactive = pTexture;
}

bool GUIButton::onHover()
{
    glm::vec2 mousePos = glm::vec2 (Input::getMouseX(), Input::getMouseY());
    glm::vec2 buttonPos = glm::vec2 (getLocalPosition().x, getLocalPosition().y);

    sf::FloatRect _bounds = _sprite.getGlobalBounds();
    if (mousePos.x < (buttonPos.x + (_bounds.width / 2.0f)) && mousePos.x > (buttonPos.x - (_bounds.width / 2.0f)) &&
        mousePos.y < (buttonPos.y + (_bounds.height / 2.0f)) &&  mousePos.y > (buttonPos.y - (_bounds.height / 2.0f))) {
        _sprite.setTexture(_active);
        return true;
    } else {
        _sprite.setTexture(_inactive);
        return false;
    }
}

bool GUIButton::onClick()
{
    if (onHover()) {
        if (!pressed && Input::GetMouseButton(0)) {
            pressed = true;
            return true;
        } else if (!Input::GetMouseButton(0)) {
            pressed = false;
            return false;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

