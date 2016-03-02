#pragma once

#include "glm.hpp"
#include "mge/collision/QuadTreeUtil.hpp"
#include "mge/util/list/DualLinkList.hpp"
#include "mge/util/list/DualLinkNode.hpp"

#define MGE_QUADTREE_SPLIT_THRESHOLD 8
using namespace QuadTreeUtil;

class QuadTreeNode
{

public:
    QuadTreeNode(int depth,QuadTreeNode* parent,float plusX, float minusX,float plusZ, float minusZ);
    ~QuadTreeNode();

    void Insert(Collider* col);
    QuadAABB& GetBoundingBox();
    DualLinkList<QuadTreeList>& GetColliders();



    void DoInternalCollisions();
    void DoRecursiveCollisionsAgainstCollider(Collider* col);
    void DoCollisionsAgainstCollider(Collider* col);//helper function

private:
    QuadAABB _boundingBox;
    QuadTreeNode *_parent = NULL;
    QuadTreeNode *_children[4];

    DualLinkList<QuadTreeList> _colliders;
    bool _hasSplit = false;
    int _depth;//for debug, will make private after

    void Split();


};


//class QuadTree
//{
//public:
////    QuadTree();
//};
