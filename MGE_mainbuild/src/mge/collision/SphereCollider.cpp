#include <mge/collision/SphereCollider.hpp>
#include <mge/collision/BoxCollider.hpp>
#include<mge/core/GameObject.hpp>
#include <glm.hpp>
#include <iostream>

SphereCollider::SphereCollider()
{
    _name = "Sphere Collider";
}

bool SphereCollider::HitTest(BoxCollider* other)
{
    return other->InternalHitTest(this);
}

bool SphereCollider::HitTest(SphereCollider* other)
{
    glm::vec3 myPos = _owner->getWorldPosition();
    glm::vec3 otherPos = other->getOwner()->getWorldPosition();

    float dist = glm::distance(myPos,otherPos);

    if(dist<=this->radius + other->radius)
        return true;
    else
        return false;

}

bool SphereCollider::HitTest(Collider* other)
{
    if(_owner == NULL || other->getOwner()==NULL)
        return false;

    return other->HitTest(this);
}

SphereCollider::~SphereCollider()
{

}
