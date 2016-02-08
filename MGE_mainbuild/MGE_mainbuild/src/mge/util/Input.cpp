#include <mge/util/Input.hpp>


bool Input::_wasPressed[101];
bool Input::_wasReleased[101];
bool Input::_mouseReleased[3];

int Input::_mouseX;
int Input::_mouseY;

bool Input::GetKey(Input::KeyCode target)
{
    return sf::Keyboard::isKeyPressed((sf::Keyboard::Key)((int)target));
    //return sf::Keyboard::isKeyPressed((int)target);
}

bool Input::GetKeyDown(Input::KeyCode target)
{
    int keyIndex = (int)target;
    return _wasPressed[keyIndex];
}

bool Input::GetKeyUp(Input::KeyCode target)
{
    int keyIndex = (int)target;
    return _wasReleased[keyIndex];
}

int Input::getMouseX()
{
    return _mouseX;
}

int Input::getMouseY()
{
    return _mouseY;
}

void Input::PollInputEvents(sf::RenderWindow* target)
{
    //first reset existing values
    _mouseReleased[0] = _mouseReleased[1] = _mouseReleased[2] = false;

    for(int i=0;i<101;++i)
    {
        _wasPressed[i] = false;
        _wasReleased[i] = false;
    }


    sf::Event event;
    while (target->pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            target->close();
            return;

        case sf::Event::KeyPressed:
            _wasPressed[(int)(event.key.code)] = true;
            break;

        case sf::Event::KeyReleased:
            _wasReleased[(int)(event.key.code)] = true;
            break;

        case sf::Event::MouseMoved:
            _mouseX = event.mouseMove.x;
            _mouseY = event.mouseMove.y;
            break;

        case sf::Event::MouseButtonReleased:
            _mouseReleased[(int)(event.mouseButton.button)] = true;
            break;



        default:
            //nothing, to get rid of one gorillion warnings about not handling all possible cases
            break;
        }
    }
}


bool Input::GetMouseButton(int target)
{
    return sf::Mouse::isButtonPressed((sf::Mouse::Button)target);
}

bool Input::GetMouseButtonUp(int target)
{
    if(target>2||target<0)
        return false;

    return _mouseReleased[target];
}






