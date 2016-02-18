#include<mge/collision/Collider.hpp>
#include<mge/collision/CollisionManager.hpp>
//#include<mge/util/list/DualLinkList.hpp>
using namespace Utils;

CollisionMTV Collider::storedMTV;

Collider::Collider()
{
    CollisionManager::_colliders.Add(this);
}

Collider::~Collider()
{
    CollisionManager::_colliders.Remove(this);
}



