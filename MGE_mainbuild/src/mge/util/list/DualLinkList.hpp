#ifndef DUAL_LINK_LIST_H
#define DUAL_LINK_LIST_H
#include <cstddef>
#include <functional>
#include <mge/util/list/DualLinkNode.hpp>

namespace Utils
{
    //high performance linked list, should only be used for engine purposes as
    //the nodes can only be part of one single list at a time and
    //safe adding/removing has a huge overhead-O(n) as opposed to unsafe adding/removing-O(1)
    //in addition, nodes have to extend the base class DualLinkNode
    template<class T>
    class DualLinkList
    {
    public:
        DualLinkNode<T>* startNode = NULL;
        DualLinkNode<T>* endNode = NULL;

        //unsafe add
        void Add(DualLinkNode<T>* target)
        {
            if(target==NULL)
                return;

            if(startNode==NULL)
            {
                target->prevNode = target->nextNode = NULL;
                startNode=target;
                endNode=target;
            }
            else
            {
                //startNode->prevNode = target;
                //target->nextNode = startNode;
                //startNode = target;

                //insert at end instead of start
                endNode->nextNode = target;
                target->prevNode = endNode;
                target->nextNode = NULL;
                endNode = target;
            }
        }

        bool Contains(DualLinkNode<T>* target)
        {
            DualLinkNode<T>* currentNode = startNode;
            while(currentNode!=NULL)
            {
                if(currentNode==target)
                    return true;

                currentNode = currentNode->nextNode;
            }

            return false;
        }

        //executes the add operation only if the list does not already contain the target
        //returns whether the adding was successful
        bool SafeAdd(DualLinkNode<T>* target)
        {
            if(Contains(target))
                return false;

            Add(target);
            return true;
        }

        //unsafe removal, assumed target is in the list
        void Remove(DualLinkNode<T>* target)
        {
            if(target==NULL||startNode==NULL)
                return;

            //if this is true, the target is the only node
            if(target->nextNode==NULL&&target->prevNode==NULL)
            {
                startNode=NULL;
                endNode = NULL;
                return;
            }

            //if this is true, the target is the last node
            //the node behind can't be null, otherwise the previous if would have ended the function
            if(target->nextNode==NULL)
            {
                target->prevNode->nextNode=NULL;
                endNode = target->prevNode;
                return;
            }

            //if this is true, the target is the first node
            //the node in front can't be null, otherwise the second if would have ended the function
            if(target->prevNode==NULL)
            {
                target->nextNode->prevNode=NULL;
                startNode = target->nextNode;
                return;
            }

            //last possibility, node is somewhere in the middle
            target->prevNode->nextNode = target->nextNode;
            target->nextNode->prevNode = target->prevNode;

        }

        //safe removal, executes only if the list contains the target to remove
        //returns whether the removal was successful
        bool SafeRemove(DualLinkNode<T>* target)
        {
            if(!Contains(target))
                return false;

            Remove(target);
            return true;
        }


        void RemoveIf(std::function<bool(T)> condition)
        {
            if(startNode==NULL)
                return;

            DualLinkNode<T>* currentNode = startNode;

            while(currentNode->nextNode!=NULL)
            {
                if(condition(currentNode->nextNode))
                {
                    if(currentNode->nextNode->nextNode!=NULL)
                    {
                        currentNode->nextNode->nextNode->prevNode = currentNode;
                        currentNode->nextNode = currentNode->nextNode->nextNode;
                    }
                    else
                    {
                        currentNode->nextNode = NULL;
                    }
                }
                else
                    currentNode = currentNode->nextNode;
            }

            //still have to deal with first node
            if(condition(startNode))
            {
                if(startNode->nextNode==NULL)
                {
                    startNode=NULL;
                }
                else
                {
                    startNode = startNode->nextNode;
                    startNode->prevNode = NULL;
                }
            }

        }

        //returns the first object in the list that satisfies the given condition, if it exists
        //returns NULL otherwise
        T ConditionalSearch(std::function<bool(T)> condition)
        {
            DualLinkNode<T>* currentNode = startNode;
            while(currentNode!=NULL)
            {
                if(condition(currentNode))
                    return currentNode;

                currentNode = currentNode->nextNode;
            }

            return NULL;
        }

        //executes the argument function for each item in the list. function is not allowed to modify the memory
        void ForEach(std::function<void(T const)> action)
        {
            DualLinkNode<T>* currentNode = startNode;

            while(currentNode!=NULL)
            {
                action((T)currentNode);

                currentNode = currentNode->nextNode;
            }
        }

        void Clear(bool safeClear = false)
        {
            if(startNode == NULL)
                return;

            if(safeClear)
            {
                //null every prev and next pointers
                DualLinkNode<T>* currentNode = startNode;

                while(currentNode->nextNode!=NULL)
                {
                    currentNode = currentNode->nextNode;
                    currentNode->prevNode->nextNode = NULL;
                    currentNode->prevNode=NULL;
                }
            }

            //null head
            startNode = NULL;
        }
    };

}
#endif // DUAL_LINK_LIST_H
