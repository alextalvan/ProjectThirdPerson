#include <mge/collision/WallCollider.hpp>
#include <mge/collision/CollisionManager.hpp>
#include <mge/core/GameObject.hpp>
#include <glm.hpp>
//#include <mge/util/Input.hpp>

WallCollider::WallCollider()
{
    _name = "Wall Collider";
    layer = CollisionManager::WALLS;
    OnCollision = [this](Collider* other, CollisionMTV& mtv) { CollisionCallback(other, mtv);};

}

void WallCollider::CollisionCallback(Collider* other, CollisionMTV& mtv)
{
    GameObject* otherOwner = other->getOwner();
    glm::vec3 relativeDir = otherOwner->getWorldPosition() - _owner->getWorldPosition();

    glm::vec3 axis = mtv.axis;

    if(glm::dot(relativeDir,axis)<0)
       axis *= -1;

    if(ignoreXaxis) axis.x = 0.0f;
    if(ignoreYaxis) axis.y = 0.0f;
    if(ignoreZaxis) axis.z = 0.0f;

    glm::mat4 newtransform = otherOwner->getWorldTransform();
    glm::vec4 loc = newtransform[3];
    loc += glm::vec4(axis * mtv.magnitude * 1.01f,0);
    //loc.y = 0;
    newtransform[3] = loc;
    otherOwner->setWorldTransform(newtransform);

}

WallCollider::~WallCollider()
{

}
