#include <mge/collision/WallCollider.hpp>
#include <mge/collision/CollisionManager.hpp>
#include <mge/core/GameObject.hpp>
#include <glm.hpp>
//#include <mge/util/Input.hpp>

WallCollider::WallCollider()
{
    _name = "Wall Collider";
    OnCollision = [this](Collider* other, CollisionMTV mtv) { CollisionCallback(other, mtv);};
    layer = CollisionManager::WALLS;
}

void WallCollider::CollisionCallback(Collider* other, CollisionMTV mtv)
{
    GameObject* otherOwner = other->getOwner();
    glm::vec3 relativeDir = otherOwner->getWorldPosition() - _owner->getWorldPosition();

    if(glm::dot(relativeDir,mtv.axis)<0)
        mtv.axis *= -1;


    glm::mat4 newtransform = otherOwner->getWorldTransform();
    glm::vec4 loc = newtransform[3];
    loc += glm::vec4(mtv.axis * mtv.magnitude * 1.01f,0);
    //loc.y = 0;
    newtransform[3] = loc;
    otherOwner->setWorldTransform(newtransform);

}

WallCollider::~WallCollider()
{

}
