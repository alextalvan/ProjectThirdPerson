#include <cassert>
#include <iostream>
#include "mge/behaviours/Component.hpp"
#include "mge/core/GameObject.hpp"

Component::Component():_owner(NULL) {}

Component::~Component()
{
    std::cout<<_name<<" was released.\n";
}


void Component::setOwner (GameObject* pOwner) {
    _owner = pOwner;
}

void Component::Destroy()
{
    Destroyable::Destroy();
    SetActive(false);
}
std::string Component::getName() const
{
    return _name;
}

void Component::Update()
{

}

GameObject* const Component::getOwner() const
{
    return _owner;
}
