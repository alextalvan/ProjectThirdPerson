#pragma once

#include "glm.hpp"
#include "mge/util/list/DualLinkList.hpp"
#include "mge/util/list/DualLinkNode.hpp"
#include <vector>
#include "mge/collision/Collider.hpp"

#define MGE_QUADTREE_SPLIT_THRESHOLD 8

class CollisionManager;

class QuadTreeNode
{
friend class CollisionManager;
public:
    void Reset(int depth,QuadTreeNode* parent,float plusX, float minusX,float plusZ, float minusZ);

    void Insert(Collider* col);
    DualLinkList<QuadTreeList>& GetColliders();



    void DoInternalCollisions();
    void DoRecursiveCollisionsAgainstCollider(Collider* col);
    void DoCollisionsAgainstCollider(Collider* col);//helper function

    static QuadTreeNode* GrabNewNode();

private:
    QuadTreeNode(int depth,QuadTreeNode* parent,float plusX, float minusX,float plusZ, float minusZ);
    ~QuadTreeNode();
    //QuadAABB _boundingBox;
    float _plusZ, _minusZ, _plusX, _minusX;
    bool OverlapCheck(const ColliderBoundingSphere& boundSphere);

    QuadTreeNode *_parent = NULL;
    QuadTreeNode *_children[4];
    DualLinkList<QuadTreeList> _colliders;
    bool _hasSplit = false;
    int _depth;//for debug, will make private after

    void Split();

    static std::vector<QuadTreeNode*> _cachedNodes;
    static unsigned int _cacheIndex;

};

