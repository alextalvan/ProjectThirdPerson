#ifndef DUAL_LINK_LIST_H
#define DUAL_LINK_LIST_H
#include <cstddef>
#include <functional>
#include <mge/util/list/DualLinkNode.hpp>

namespace Utils
{
    //extremely high performance linked list
    //the nodes can only be part of one single list at a time and
    //safe adding/removing has a huge overhead-O(n) as opposed to regular adding/removing-O(1)
    //nodes have to extend the base class DualLinkNode
    //when using this, you may use Remove() while iterating through the list without the fear of breaking anything.
    template<class T>
    class DualLinkList
    {
    private:
        int _elementCount = 0;
    public:
        DualLinkNode<T>* startNode = NULL;
        DualLinkNode<T>* endNode = NULL;

        int GetCount()
        {
            return _elementCount;
        }

        DualLinkNode<T>* const GetAt (int i)
        {
            if(i>=_elementCount)
                throw "DualLinkList index out of range\n";


           DualLinkNode<T>* currentNode = startNode;
           for(int j=0;j<i;++j)
           {
               currentNode = currentNode->nextNode;
           }

           return currentNode;
        }

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
                _elementCount++;
            }
            else
            {
                //insert at end instead of start
                endNode->nextNode = target;
                target->prevNode = endNode;
                target->nextNode = NULL;
                endNode = target;
                _elementCount++;
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
                _elementCount--;
                return;
            }

            //if this is true, the target is the last node
            //the node behind can't be null, otherwise the previous if would have ended the function
            if(target->nextNode==NULL)
            {
                target->prevNode->nextNode=NULL;
                endNode = target->prevNode;
                _elementCount--;
                return;
            }

            //if this is true, the target is the first node
            //the node in front can't be null, otherwise the second if would have ended the function
            if(target->prevNode==NULL)
            {
                target->nextNode->prevNode=NULL;
                startNode = target->nextNode;
                _elementCount--;
                return;
            }

            //last possibility, node is somewhere in the middle
            target->prevNode->nextNode = target->nextNode;
            target->nextNode->prevNode = target->prevNode;
            _elementCount--;

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
                    _elementCount--;
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
                _elementCount--;
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
            _elementCount = 0;
        }
    };



    //a literal copy of the first list class because you can't template variables on c++11
    //multiple inheritance was needed, but I decided to duplicate this class insead
    template<class T>
    class DualLinkList2
    {
    private:
        int _elementCount = 0;
    public:
        DualLinkNode2<T>* startNode = NULL;
        DualLinkNode2<T>* endNode = NULL;

        int GetCount()
        {
            return _elementCount;
        }

        DualLinkNode2<T>* const GetAt (int i)
        {
            if(i>=_elementCount)
                throw "DualLinkList index out of range\n";


           DualLinkNode2<T>* currentNode = startNode;
           for(int j=0;j<i;++j)
           {
               currentNode = currentNode->nextNode;
           }

           return currentNode;
        }

        //unsafe add
        void Add(DualLinkNode2<T>* target)
        {
            if(target==NULL)
                return;

            if(startNode==NULL)
            {
                target->prevNode = target->nextNode = NULL;
                startNode=target;
                endNode=target;
                _elementCount++;
            }
            else
            {
                //insert at end instead of start
                endNode->nextNode = target;
                target->prevNode = endNode;
                target->nextNode = NULL;
                endNode = target;
                _elementCount++;
            }
        }

        bool Contains(DualLinkNode2<T>* target)
        {
            DualLinkNode2<T>* currentNode = startNode;
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
        bool SafeAdd(DualLinkNode2<T>* target)
        {
            if(Contains(target))
                return false;

            Add(target);
            return true;
        }

        //unsafe removal, assumed target is in the list
        void Remove(DualLinkNode2<T>* target)
        {
            if(target==NULL||startNode==NULL)
                return;

            //if this is true, the target is the only node
            if(target->nextNode==NULL&&target->prevNode==NULL)
            {
                startNode=NULL;
                endNode = NULL;
                _elementCount--;
                return;
            }

            //if this is true, the target is the last node
            //the node behind can't be null, otherwise the previous if would have ended the function
            if(target->nextNode==NULL)
            {
                target->prevNode->nextNode=NULL;
                endNode = target->prevNode;
                _elementCount--;
                return;
            }

            //if this is true, the target is the first node
            //the node in front can't be null, otherwise the second if would have ended the function
            if(target->prevNode==NULL)
            {
                target->nextNode->prevNode=NULL;
                startNode = target->nextNode;
                _elementCount--;
                return;
            }

            //last possibility, node is somewhere in the middle
            target->prevNode->nextNode = target->nextNode;
            target->nextNode->prevNode = target->prevNode;
            _elementCount--;

        }

        //safe removal, executes only if the list contains the target to remove
        //returns whether the removal was successful
        bool SafeRemove(DualLinkNode2<T>* target)
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

            DualLinkNode2<T>* currentNode = startNode;

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
                    _elementCount--;
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
                _elementCount--;
            }

        }

        //returns the first object in the list that satisfies the given condition, if it exists
        //returns NULL otherwise
        T ConditionalSearch(std::function<bool(T)> condition)
        {
            DualLinkNode2<T>* currentNode = startNode;
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
            DualLinkNode2<T>* currentNode = startNode;

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
                DualLinkNode2<T>* currentNode = startNode;

                while(currentNode->nextNode!=NULL)
                {
                    currentNode = currentNode->nextNode;
                    currentNode->prevNode->nextNode = NULL;
                    currentNode->prevNode=NULL;
                }
            }

            //null head
            startNode = NULL;
            _elementCount = 0;
        }
    };


}
#endif // DUAL_LINK_LIST_H
