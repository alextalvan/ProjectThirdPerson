#include <mge/collision/QuadTree.hpp>
#include <mge/collision/CollisionManager.hpp>
#include <mge/lua/LuaScript.hpp>


glm::vec3 red = glm::vec3(1,0,0);

QuadTreeNode::QuadTreeNode(int depth,QuadTreeNode* parent,float plusX, float minusX,float plusZ, float minusZ) : _boundingBox(plusX, minusX, plusZ, minusZ)
{
    _depth = depth;
    _parent = parent;

//    for(int i=0;i<4;++i)
//        _children[i] = NULL;
}

QuadTreeNode::~QuadTreeNode()
{
    if(_hasSplit)
    {
        for(int i=0;i<4;++i)
            delete _children[i];
    }
}

QuadAABB& QuadTreeNode::GetBoundingBox()
{
    return _boundingBox;
}

void QuadTreeNode::Insert(Collider* col)
{
    if(_hasSplit)
    {
        int overlaps = 0;
        QuadTreeNode* target;

        for(int i=0;i<4;++i)
        {
            if((_children[i]->GetBoundingBox()).OverlapCheck(col->GetBoundingSphere()))
            {
                target = _children[i];
                overlaps++;

                if(overlaps>1)//collider overlaps with 2 or more sub-trees, so it is left inside the parent tree
                {
                    _colliders.Add((DualLinkNode<QuadTreeList>*)col);
                    col->quadTreeOwner = this;
                    return;
                }

            }
        }

        //if(overlaps==1)//the collider fits in one exact sub bounding box, so it is inserted in it
        target->Insert(col);
    }
    else
    {
        _colliders.Add((DualLinkNode<QuadTreeList>*)col);
        col->quadTreeOwner = this;

        if(_colliders.GetCount() == MGE_QUADTREE_SPLIT_THRESHOLD)
        {
            Split();//after a split is done, all colliders have to be reinserted, which will now take the children nodes into account
            DualLinkNode<QuadTreeList>* cn = _colliders.startNode;
            DualLinkNode<QuadTreeList>* nn = cn->nextNode;
            _colliders.Clear();

            while(nn!=NULL)
            {
                Insert((Collider*)cn);
                cn = nn;
                nn = nn->nextNode;
            }
            //at the end, the last node still has to be reprocessed
            Insert((Collider*)cn);
        }

    }

}

void QuadTreeNode::Split()
{
    _hasSplit = true;

    //bounds are given as vec4(plusX,minX,plusZ,minZ)
    glm::vec4 bounds = _boundingBox.GetBounds();
    float halfX = bounds.y + (bounds.x - bounds.y) * 0.5f;
    float halfZ = bounds.w + (bounds.z - bounds.w) * 0.5f;

    //minusx + minusz
    _children[0] = new QuadTreeNode(_depth+1,this,halfX,bounds.y,halfZ,bounds.w);

    //plusx + minz
    _children[1] = new QuadTreeNode(_depth+1,this,bounds.x,halfX,halfZ,bounds.w);

    //plusx + plusz
    _children[2] = new QuadTreeNode(_depth+1,this,bounds.x,halfX,bounds.z,halfZ);

    //minx + plusz
    _children[3] = new QuadTreeNode(_depth+1,this,halfX,bounds.y,bounds.z,halfZ);
}

 DualLinkList<QuadTreeList>& QuadTreeNode::GetColliders()
 {
     return _colliders;
 }

void QuadTreeNode::DoInternalCollisions()
{
    DualLinkNode<QuadTreeList>* currentNodei = _colliders.startNode;
    DualLinkNode<QuadTreeList>* currentNodej;
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

            if(!(colliderj->IsActive()) || !CollisionManager::CheckLayerInteraction(collideri->layer,colliderj->layer))
            {
                currentNodej = currentNodej->nextNode;
                continue;
            }

            //at this point the conditions for calculating the collision are met
            bool result = collideri->HitTest(colliderj);
            //debug
            CollisionManager::debugInfo.tests++;

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

    if(_hasSplit)
    {
        for(int i=0;i<4;++i)
            _children[i]->DoInternalCollisions();
    }
}

void QuadTreeNode::DoCollisionsAgainstCollider(Collider* col)
{
    if(!col->IsActive())
        return;

    DualLinkNode<QuadTreeList>* cn = _colliders.startNode;

    while(cn!=NULL)
    {
        Collider* ccol = (Collider*)cn;
        if(ccol->IsActive()&&CollisionManager::CheckLayerInteraction(ccol->layer,col->layer))
        {
            bool result = col->HitTest(ccol);
            //debug
            CollisionManager::debugInfo.tests++;

            //trigger callbacks
            if(result)
            {
                CollisionMTV mtv = Collider::storedMTV;

                if(col->OnCollision!=NULL)
                {
                    col->OnCollision(ccol,mtv);

                }

                if(ccol->OnCollision!=NULL)
                {
                    //mtv.axis *= -1;
                    ccol->OnCollision(col,mtv);
                }

                //lua callbacks
                LuaScript* l = col->getOwner()->GetComponent<LuaScript>();
                if(l!=NULL)
                    l->InvokeCollisionCallback(ccol->getOwner());

                LuaScript* l1 = ccol->getOwner()->GetComponent<LuaScript>();
                if(l1!=NULL)
                    l1->InvokeCollisionCallback(col->getOwner());

            }
        }

        cn = cn->nextNode;
    }
}

void QuadTreeNode::DoRecursiveCollisionsAgainstCollider(Collider* col)
{
    if(col->quadTreeOwner != this)
    {
        DoCollisionsAgainstCollider(col);
    }

    if(_hasSplit)
    {
        for(int i=0;i<4;++i)
        {
            if(_children[i]->GetBoundingBox().OverlapCheck(col->GetBoundingSphere()))
                _children[i]->DoRecursiveCollisionsAgainstCollider(col);
        }

    }
}


