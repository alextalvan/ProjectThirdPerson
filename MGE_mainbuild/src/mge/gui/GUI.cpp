#include "mge/gui/GUI.hpp"

using namespace std;

GUI::GUI(sf::RenderWindow * pWindow, std::string pName, float pPosX, float pPosY)
:   GameObject(pName, glm::vec3(pPosX, pPosY, 0.0f)), _window(pWindow)
{

}

void GUI::draw(GUI& target)
{
	drawCurrent(target);

	int childCount = GetChildCount();
    if (childCount < 1) return;

    //note that with a loop like this, deleting children during rendering is not a good idea :)
    for (int i = 0; i < childCount; i++) {
        GUI* r = (GUI*)GetChildAt(i);
        r->draw(*r);
    }
}

void GUI::drawCurrent(GUI& target)
{

}


GUI::~GUI()
{
	//dtor
}
