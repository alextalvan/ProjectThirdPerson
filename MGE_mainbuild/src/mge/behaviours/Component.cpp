#include <cassert>
#include <iostream>
#include "mge/behaviours/Component.hpp"
#include "mge/core/GameObject.hpp"

Component::Component():_owner(NULL) {}

Component::~Component()
{
    std::cout<<_name<<" was released.\n";
}


void Component::setOwner (GameObject* owner)
{
    if(owner==_owner)
        return;

    if(_owner!=NULL)
        _owner->_innerRemove(this);

    if(owner!=NULL)
        owner->_innerAdd(this);

    _owner = owner;
}

void Component::Destroy()
{
    Destroyable::Destroy();
    SetActive(false);
    if(_owner!=NULL && !_owner->isDestroyed())
        setOwner(NULL);
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
