#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H
#include<mge/util/list/DualLinkList.hpp>
#include<mge/core/AbstractGame.hpp>
#include<mge/core/GameObject.hpp>
#include "glm.hpp"
using namespace Utils;

class Collider;
//class GameObject;

struct RaycastInfo
{
    GameObject* object;
    glm::vec3 impactPoint;
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
        LAYER2,
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

private:
    static bool collisionMatrix[10][10];
    static DualLinkList<Collider> _colliders;
    static void Initialize();
    static void DoCollisions();

    static bool Raycast(glm::vec3 origin, glm::vec3 direction, RaycastInfo& output);
};



#endif // COLLISIONMANAGER_H
