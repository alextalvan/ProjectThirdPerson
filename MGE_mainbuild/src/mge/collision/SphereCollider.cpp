#include <mge/collision/SphereCollider.hpp>
#include <mge/collision/BoxCollider.hpp>
#include <mge/collision/TerrainCollider.hpp>
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
    return (this->BoundingSphereCheck(other));
}

void SphereCollider::RefreshBoundingSphere()
{
    if(_owner==NULL)
        return;

    glm::mat4 objMat = _owner->getWorldTransform();
    _bound.position = glm::vec3(objMat[3]);

    float maxScale = glm::length(objMat[0]);
    float newScale = glm::length(objMat[1]);
    if(newScale > maxScale) maxScale = newScale;
    newScale = glm::length(objMat[2]);
    if(newScale > maxScale) maxScale = newScale;

    _bound.radius = maxScale * radius;
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

    vec3 diff = _bound.position - ray.origin;

    float a = dot(ray.direction,diff);

    if(a<0.0f) //ray points opposite of sphere
        return false;

    float diffSquared = dot(diff,diff);
    float radSquared = _bound.radius * _bound.radius;

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

bool SphereCollider::HitTest(TerrainCollider* other)
{
    return other->InternalHitTest(this);
}

