#include "mge/gui/GUI.hpp"
#include "mge/util/list/DualLinkNode.hpp"

using namespace std;

GUI::GUI(std::string pName, float pPosX, float pPosY)
:   GameObject(pName, glm::vec3(pPosX, pPosY, 0.0f))
{

}

void GUI::SetGUIActive(bool active)
{
    _active = active;
}

void GUI::DrawTo(sf::RenderTarget& t)
{
    if (_active) {
        InnerDraw(t);


        DualLinkNode2<ChildList>* g = _children.startNode;
        while(g!=NULL)
        {
            GUI* cast = (GUI*)g;

            if(cast->IsActive())
                cast->DrawTo(t);

            g = g->nextNode;
        }
    }
}

void GUI::InnerDraw(sf::RenderTarget& t)
{

}

void GUI::Refresh2DTransform()
{

}

GUI::~GUI()
{
	//dtor
}
