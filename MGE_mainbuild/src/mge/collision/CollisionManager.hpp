#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H
#include<mge/util/list/DualLinkList.hpp>
#include<mge/core/AbstractGame.hpp>
#include<mge/core/GameObject.hpp>
#include "glm.hpp"
using namespace Utils;

class Collider;
class RaycastList;//dummy class for list templates
//class GameObject;

struct RaycastInfo
{
    GameObject* object;
    glm::vec3 impactPoint;
    float distance;
};

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
    Ray(glm::vec3 orig,glm::vec3 dir) : origin(orig), direction(dir) {}
};

class CollisionManager
{
friend class Collider;
friend class AbstractGame;
public:
    //up to 10 layers of collision
    //rename these when needed
    enum COLLISION_LAYERS
    {
        DEFAULT,
        WALLS,
        IGNORE_WALLS,
        LAYER3,
        LAYER4,
        LAYER5,
        LAYER6,
        LAYER7,
        LAYER8,
        LAYER9,
    };

    static void SetLayerInteraction(COLLISION_LAYERS l1,COLLISION_LAYERS l2, bool value);
    static bool CheckLayerInteraction(COLLISION_LAYERS l1,COLLISION_LAYERS l2);

    static bool Raycast(const Ray& ray, RaycastInfo& output);//Ray passed by reference but only for performance reasons, so it is read only

private:
    static bool collisionMatrix[10][10];
    static DualLinkList<Collider> _colliders;
    static void Initialize();
    static void DoCollisions();


    static DualLinkList<RaycastList> _raycastTargets;

};



#endif // COLLISIONMANAGER_H
