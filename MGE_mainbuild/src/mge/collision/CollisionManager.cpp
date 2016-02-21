#include<mge/collision/CollisionManager.hpp>
#include<mge/util/list/DualLinkNode.hpp>
#include<mge/collision/Collider.hpp>
#include<mge/lua/LuaScript.hpp>
#include<mge/core/GameObject.hpp>
using namespace Utils;

bool CollisionManager::collisionMatrix[10][10];
DualLinkList<Collider> CollisionManager::_colliders;
DualLinkList<RaycastList> CollisionManager::_raycastTargets;

void CollisionManager::Initialize()
{
    for(int i=0;i<10;++i)
        for(int j=0;j<10;++j)
            CollisionManager::collisionMatrix[i][j] = true;

    SetLayerInteraction(CollisionManager::WALLS,CollisionManager::WALLS,false);
}


bool CollisionManager::CheckLayerInteraction(COLLISION_LAYERS l1,COLLISION_LAYERS l2)
{
    return CollisionManager::collisionMatrix[(int)l1][(int)l2];
}

void CollisionManager::SetLayerInteraction(COLLISION_LAYERS l1,COLLISION_LAYERS l2,bool value)
{
    CollisionManager::collisionMatrix[(int)l1][(int)l2] = value;
    CollisionManager::collisionMatrix[(int)l2][(int)l1] = value;
}

void CollisionManager::DoCollisions()
{
    if(CollisionManager::_colliders.startNode==NULL)
        return;


    DualLinkNode<Collider>* currentNodei = CollisionManager::_colliders.startNode;
    DualLinkNode<Collider>* currentNodej;
    Collider* collideri;
    Collider* colliderj;

    while(currentNodei !=NULL)
    {
        collideri = (Collider*) currentNodei;

        if(!(collideri->IsActive()))
        {
            currentNodei = currentNodei->nextNode;
            continue;
        }

        currentNodej = currentNodei->nextNode;

        while(currentNodej!=NULL)
        {
            colliderj = (Collider*) currentNodej;

            if(!(colliderj->IsActive()) || !CheckLayerInteraction(collideri->layer,colliderj->layer))
            {
                currentNodej = currentNodej->nextNode;
                continue;
            }

            //at this point the conditions for calculating the collision are met
            bool result = collideri->HitTest(colliderj);

            //trigger callbacks
            if(result)
            {
                CollisionMTV mtv = Collider::storedMTV;

                if(collideri->OnCollision!=NULL)
                {
                    collideri->OnCollision(colliderj,mtv);

                }

                if(colliderj->OnCollision!=NULL)
                {
                    //mtv.axis *= -1;
                    colliderj->OnCollision(collideri,mtv);
                }

                //lua callbacks

                LuaScript* l = collideri->getOwner()->GetComponent<LuaScript>();
                if(l!=NULL)
                    l->InvokeCollisionCallback(colliderj->getOwner());

                LuaScript* l1 = colliderj->getOwner()->GetComponent<LuaScript>();
                if(l1!=NULL)
                    l1->InvokeCollisionCallback(collideri->getOwner());

            }

            currentNodej = currentNodej->nextNode;

        }

        currentNodei = currentNodei->nextNode;
    }
}

bool CollisionManager::Raycast(const Ray& ray, RaycastInfo& output)
{
    DualLinkNode<RaycastList>* cn = CollisionManager::_raycastTargets.startNode;

    GameObject* bestTarget = NULL;
    float bestDistance = FLT_MAX;

    float distance;

    while(cn!=NULL)
    {
        Collider* colCast = (Collider*)cn;

        if(colCast->IsActive())
        {
            bool result = colCast->RayTest(ray,distance);
            if(result && distance < bestDistance)
            {
                bestDistance = distance;
                bestTarget = colCast->getOwner();
            }
        }

        cn = cn->nextNode;
    }

    if(bestTarget == NULL)
        return false;
    else
    {
        output.object = bestTarget;
        output.impactPoint = ray.origin + ray.direction * bestDistance;
        return true;
    }
}
