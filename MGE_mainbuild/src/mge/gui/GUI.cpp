#include "mge/gui/GUI.hpp"
#include "mge/util/list/DualLinkNode.hpp"

using namespace std;

GUI::GUI(std::string pName, float pPosX, float pPosY)
:   GameObject(pName, glm::vec3(pPosX, pPosY, 0.0f))
{

}

void GUI::DrawTo(sf::RenderTarget& t)
{
    InnerDraw(t);


    DualLinkNode2<ChildList>* g = _children.startNode;
    while(g!=NULL)
    {
        ((GUI*)g)->DrawTo(t);
        g = g->nextNode;
    }
}

void GUI::InnerDraw(sf::RenderTarget& t)
{

}


/*
void GUI::drawCurrent(GUI& target)
{

}
*/

GUI::~GUI()
{
	//dtor
}
