#include<mge/collision/Collider.hpp>
#include<mge/collision/CollisionManager.hpp>
//#include<mge/util/list/DualLinkList.hpp>
using namespace Utils;

CollisionMTV Collider::storedMTV;

Collider::Collider()
{
    CollisionManager::_colliders.Add((DualLinkNode<Collider>*)this);
}

Collider::~Collider()
{
    CollisionManager::_colliders.Remove((DualLinkNode<Collider>*)this);

    if(!ignoreRaycast)
        CollisionManager::_raycastTargets.Remove((DualLinkNode<RaycastList>*)this);
}


void Collider::SetRaycastable(bool val)
{
    ignoreRaycast = !val;

    if(val)
        CollisionManager::_raycastTargets.Add((DualLinkNode<RaycastList>*)this);
    else
        CollisionManager::_raycastTargets.Remove((DualLinkNode<RaycastList>*)this);
}
