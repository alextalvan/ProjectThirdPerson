#ifndef COLLIDER_H
#define COLLIDER_H
#include <mge/behaviours/Component.hpp>
#include <mge/util/list/DualLinkNode.hpp>
#include <mge/collision/CollisionManager.hpp>
#include <functional>
#include <glm.hpp>
using namespace Utils;

class BoxCollider;
class SphereCollider;
class CollisionManager;

//absolute minimum translation vector - is calculated and saved after a collision test is invoked,
//then the collision manager passes it in the OnCollision callback
struct CollisionMTV
{
    glm::vec3 axis = glm::vec3(1,0,0);//this always points TOWARDS the object the OnCollision callback is called on
    float magnitude = 0;
};

//for raycasts - list template dummy class
class RaycastList;

class Collider : public Component, private DualLinkNode<Collider>, private DualLinkNode<RaycastList>
{
friend class CollisionManager;
public:
    std::function<void(Collider*,CollisionMTV)> OnCollision = NULL;
    CollisionManager::COLLISION_LAYERS layer = CollisionManager::DEFAULT;

    virtual bool HitTest(BoxCollider* other) = 0;
    virtual bool HitTest(SphereCollider* other) = 0;
    virtual bool HitTest(Collider* other) = 0;

    //Ray passed by reference for performance reasons, so should be read only
    //The impact point will be modified if the ray intersects the collider
    virtual bool RayTest(const Ray& ray, float& distance) = 0;
    void SetRaycastable(bool val);

protected:
    Collider();
    virtual ~Collider();
    static CollisionMTV storedMTV;

private:
    bool ignoreRaycast = true;
};



#endif // COLLIDER_H
