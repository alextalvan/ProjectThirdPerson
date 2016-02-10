#include<mge/core/Destroyable.hpp>
#include<mge/util/list/SingleLinkNode.hpp>
#include<mge/util/list/SingleLinkList.hpp>
#include<mge/core/GameObject.hpp>
#include<iostream>
using namespace Utils;

//int testCount = 0;

SingleLinkList<Destroyable> Destroyable::_objectList;

Destroyable::Destroyable()
{
    _objectList.Add(this);
    //testCount++;
}

void Destroyable::Destroy()
{
    _toBeDestroyed = true;
}

bool Destroyable::isDestroyed()
{
    return _toBeDestroyed;
}

Destroyable::~Destroyable()
{
    //testCount--;
}

void Destroyable::CollectGarbage()
{

    SingleLinkNode<Destroyable>* currentNode = _objectList.startNode;
    //nothing to check
    if(currentNode==NULL)
        return;

    while(currentNode->nextNode!=NULL)
    {
        Destroyable* temp = (Destroyable*) (currentNode->nextNode);

        if(temp->_toBeDestroyed)
        {
            //merge list
            if(currentNode->nextNode->nextNode!=NULL)
                currentNode->nextNode = currentNode->nextNode->nextNode;
            else
                currentNode->nextNode = NULL;

            //deletion
            delete temp;
        }
        else
            currentNode = currentNode->nextNode;
    }


    //handle first item of list

    Destroyable* firstObj = (Destroyable*) (_objectList.startNode);
    if(firstObj->_toBeDestroyed)
    {
        if(firstObj->nextNode!=NULL)
        {
            _objectList.startNode = firstObj->nextNode;
        }
        else
            _objectList.startNode = NULL;

        delete firstObj;
    }

    //std::cout<<"escaped cg loop.\n";
    //std::cout<<testCount<<"\n";
}
