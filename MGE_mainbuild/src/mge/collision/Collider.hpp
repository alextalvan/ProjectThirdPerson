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
class QuadTreeNode;

//absolute minimum translation vector - is calculated and saved after a collision test is invoked,
//then the collision manager passes it in the OnCollision callback
struct CollisionMTV
{
    glm::vec3 axis = glm::vec3(1,0,0);//this always points TOWARDS the object the OnCollision callback is called on
    float magnitude = 0;
};

struct ColliderBoundingSphere
{
    glm::vec3 position;
    float radius;
};


//for raycasts - list template dummy class
class RaycastList;
//for the quadtree
class QuadTreeList;

class Collider : public Component, private DualLinkNode<Collider>, private DualLinkNode<RaycastList>, public DualLinkNode<QuadTreeList>
{
friend class CollisionManager;
friend class QuadTreeNode;
public:
    std::function<void(Collider*,CollisionMTV&)> OnCollision = NULL;
    CollisionManager::COLLISION_LAYERS layer = CollisionManager::DEFAULT;

    virtual bool HitTest(BoxCollider* other) = 0;
    virtual bool HitTest(SphereCollider* other) = 0;
    virtual bool HitTest(Collider* other) = 0;

    //Ray passed by reference for performance reasons, so should be read only
    //The impact point will be modified if the ray intersects the collider
    virtual bool RayTest(const Ray& ray, float& distance) = 0;
    void SetRaycastable(bool val);

    const ColliderBoundingSphere& GetBoundingSphere();
    virtual void RefreshBoundingSphere() = 0;//all collider bounding spheres will be cached before a round of collision checking
    bool BoundingSphereCheck(Collider* other);

    bool IsStatic();
    void SetStatic(bool val);



    //the quad tree node this collider has been put into
    //void RecursiveQuadTreeCollisions(QuadTreeNode* target);
protected:
    Collider();
    virtual ~Collider();

    void setOwner (GameObject* owner) override;

    static CollisionMTV storedMTV;
    ColliderBoundingSphere _bound;

private:
    bool ignoreRaycast = true;
    QuadTreeNode* quadTreeOwner;
    //space partition
    bool isStatic = true;//this will be used to avoid computing the bounding radius all the time if the collider is marked as static

};



#endif // COLLIDER_H
