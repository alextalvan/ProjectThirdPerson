#include "mge/gui/GUI.hpp"
#include "mge/util/list/DualLinkNode.hpp"
#include "mge/core/Renderer.hpp"

using namespace std;

GUI::GUI(std::string pName, float pPosX, float pPosY)
:   GameObject(pName, glm::vec3(pPosX, pPosY, 0.0f))
{

}

void GUI::SetGUIActive(bool active)
{
    SetActive(active);
}

void GUI::startFade(bool state, int speed)
{
    _fade = true;
    _fadeState = state;
    _fadeSpeed = speed;
}

void GUI::fade()
{

}

void GUI::DrawTo(sf::RenderTarget& t)
{
    InnerDraw(t);


    DualLinkNode2<ChildList>* g = _children.startNode;
    while(g!=NULL)
    {
        GUI* cast = (GUI*)g;

        if(cast->IsActive())
        {
            cast->DrawTo(t);
            Renderer::debugInfo.drawCallCount++;
        }


        g = g->nextNode;
    }
}

void GUI::Update()
{
    fade();
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
