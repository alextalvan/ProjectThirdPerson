#include <mge/collision/QuadTree.hpp>
#include <mge/collision/CollisionManager.hpp>
#include <mge/lua/LuaScript.hpp>


std::vector<QuadTreeNode*> QuadTreeNode::_cachedNodes;
unsigned int QuadTreeNode::_cacheIndex = 0;

QuadTreeNode::QuadTreeNode(int depth,QuadTreeNode* parent,float plusX, float minusX,float plusZ, float minusZ)
{
    _depth = depth;
    _parent = parent;
    _plusX = plusX;
    _minusX = minusX;
    _plusZ = plusZ;
    _minusZ = minusZ;
}

QuadTreeNode::~QuadTreeNode()
{
    /*
    if(_hasSplit)
    {
        for(int i=0;i<4;++i)
            delete _children[i];
    }*/
}

void QuadTreeNode::Reset(int depth,QuadTreeNode* parent,float plusX, float minusX,float plusZ, float minusZ)
{
    _depth = depth;
    _parent = parent;
    _plusX = plusX;
    _minusX = minusX;
    _plusZ = plusZ;
    _minusZ = minusZ;
    _hasSplit = false;
    _colliders.Clear();
}

QuadTreeNode* QuadTreeNode::GrabNewNode()
{
    if(_cacheIndex==_cachedNodes.size())
    {
        QuadTreeNode* newNode = new QuadTreeNode(0,NULL,0,0,0,0);
        _cachedNodes.push_back(newNode);
        _cacheIndex++;
        return newNode;
    }
    else
    {
        _cacheIndex++;
        return _cachedNodes[_cacheIndex-1];
    }
}

bool QuadTreeNode::OverlapCheck(const ColliderBoundingSphere& boundSphere)
{
    if(boundSphere.position.z + boundSphere.radius < _minusZ )
        return false;

    if(boundSphere.position.z - boundSphere.radius > _plusZ )
        return false;

    if(boundSphere.position.x + boundSphere.radius < _minusX )
        return false;

    if(boundSphere.position.x - boundSphere.radius > _plusX )
        return false;

    return true;
}

void QuadTreeNode::Insert(Collider* col)
{
    if(_hasSplit)
    {
        int overlaps = 0;
        QuadTreeNode* target;

        for(int i=0;i<4;++i)
        {
            if(_children[i]->OverlapCheck(col->GetBoundingSphere()))
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

    float halfX = _minusX + (_plusX - _minusX) * 0.5f;
    float halfZ = _minusZ + (_plusZ - _minusZ) * 0.5f;

    //minusx + minusz
    //_children[0] = new QuadTreeNode(_depth+1,this,halfX,_minusX,halfZ,_minusZ);
    _children[0] = GrabNewNode();
    _children[0]->Reset(_depth+1,this,halfX,_minusX,halfZ,_minusZ);

    //plusx + minz
    //_children[1] = new QuadTreeNode(_depth+1,this,_plusX,halfX,halfZ,_minusZ);
    _children[1] = GrabNewNode();
    _children[1]->Reset(_depth+1,this,_plusX,halfX,halfZ,_minusZ);

    //plusx + plusz
    //_children[2] = new QuadTreeNode(_depth+1,this,_plusX,halfX,_plusZ,halfZ);
    _children[2] = GrabNewNode();
    _children[2]->Reset(_depth+1,this,_plusX,halfX,_plusZ,halfZ);

    //minx + plusz
    //_children[3] = new QuadTreeNode(_depth+1,this,halfX,_minusX,_plusZ,halfZ);
    _children[3] = GrabNewNode();
    _children[3]->Reset(_depth+1,this,halfX,_minusX,_plusZ,halfZ);
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
            if(_children[i]->OverlapCheck(col->GetBoundingSphere()))
                _children[i]->DoRecursiveCollisionsAgainstCollider(col);
        }

    }
}


