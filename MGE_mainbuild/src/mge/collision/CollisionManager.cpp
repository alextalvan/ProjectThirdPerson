#include<mge/collision/CollisionManager.hpp>
#include<mge/util/list/DualLinkNode.hpp>
#include<mge/collision/Collider.hpp>
#include<mge/lua/LuaScript.hpp>
#include<mge/core/GameObject.hpp>
#include<mge/core/Time.hpp>
#include<mge/collision/QuadTree.hpp>
using namespace Utils;

bool CollisionManager::collisionMatrix[10][10];
DualLinkList<Collider> CollisionManager::_colliders;
DualLinkList<RaycastList> CollisionManager::_raycastTargets;
QuadTreeNode* CollisionManager::_quadTreeRoot = NULL;

CollisionDebugInfo CollisionManager::debugInfo;

void CollisionManager::Initialize()
{
    for(int i=0;i<10;++i)
        for(int j=0;j<10;++j)
            CollisionManager::collisionMatrix[i][j] = true;

    SetLayerInteraction(CollisionManager::WALLS,CollisionManager::WALLS,false);
    SetLayerInteraction(CollisionManager::WALLS,CollisionManager::IGNORE_WALLS,false);
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
    debugInfo.tests = 0;

    if(CollisionManager::_colliders.startNode==NULL)
        return;

    Time::update();
    float startTime = Time::now();


    DualLinkNode<Collider>* currentNodei = CollisionManager::_colliders.startNode;
    //DualLinkNode<Collider>* currentNodej;
    Collider* collideri;
    //Collider* colliderj;

    //first, rebuild the bounding spheres for all colliders
    //TODO: enable static colliders that never refresh their bounding spheres
    while(currentNodei !=NULL)
    {
        collideri = (Collider*)currentNodei;
        if(!collideri->IsStatic())
            collideri->RefreshBoundingSphere();

        currentNodei = currentNodei->nextNode;
    }

    //for now, releasing the entire old quadtree and creating a new one
    //if(_quadTreeRoot!=NULL)
    //   delete _quadTreeRoot;

    //reset quad tree cache index
    QuadTreeNode::_cacheIndex = 0;

    _quadTreeRoot = QuadTreeNode::GrabNewNode();
    _quadTreeRoot->Reset(0,NULL,10000,-10000,10000,-10000);
    //_quadTreeRoot = new QuadTreeNode(0,NULL,800,-800,800,-800);

    currentNodei = CollisionManager::_colliders.startNode;
    while(currentNodei !=NULL)
    {
        _quadTreeRoot->Insert((Collider*)currentNodei);
        currentNodei = currentNodei->nextNode;
    }

    //std::cout<<QuadTreeNode::_cacheIndex<<"\n";
    _quadTreeRoot->DoInternalCollisions();

    currentNodei = CollisionManager::_colliders.startNode;
    while(currentNodei !=NULL)
    {
        Collider* col = (Collider*)currentNodei;
        col->quadTreeOwner->DoRecursiveCollisionsAgainstCollider(col);
        //_quadTreeRoot->DoRecursiveCollisionsAgainstCollider((Collider*)currentNodei);
        currentNodei = currentNodei->nextNode;
    }
    /*
    currentNodei = CollisionManager::_colliders.startNode;
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

            if(!(colliderj->IsActive()) || !CheckLayerInteraction(collideri->layer,colliderj->layer) || !QuadTreeNode::BroadTest(collideri,colliderj))
            {
                currentNodej = currentNodej->nextNode;
                continue;
            }

            //at this point the conditions for calculating the collision are met
            bool result = collideri->HitTest(colliderj);
            //debug
            debugInfo.tests++;

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


    */
    Time::update();
    debugInfo.time = (Time::now() - startTime)* 1000.0f;

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
        output.distance = bestDistance;
        return true;
    }
}
