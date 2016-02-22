#include <mge/collision/SphereCollider.hpp>
#include <mge/collision/BoxCollider.hpp>
#include<mge/core/GameObject.hpp>
#include <glm.hpp>
#include <iostream>

SphereCollider::SphereCollider(float r, bool ignorRay)
{
    _name = "Sphere Collider";
    radius = r;

    if(!ignorRay)
        SetRaycastable(true);
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

bool SphereCollider::RayTest(const Ray& ray, float& distance)
{
    using namespace glm;
    //algorithm taken from the 3D Math Primer by Fletcher Dunn, 1st edition

    vec3 diff = _owner->getWorldPosition() - ray.origin;

    float a = dot(ray.direction,diff);

    if(a<0.0f) //ray points opposite of sphere
        return false;

    float diffSquared = dot(diff,diff);
    float radSquared = radius * radius;

    //check if the ray originates inside the sphere, we will consider this a miss
    if(diffSquared < radSquared)
        return false;

    float fSquared = radSquared - diffSquared + a * a;

    if(fSquared < 0.0f) //no intersection
        return false;
    else
    {
        distance = a - glm::sqrt(fSquared);
        return true;
    }
    //return false;
}

SphereCollider::~SphereCollider()
{

}

