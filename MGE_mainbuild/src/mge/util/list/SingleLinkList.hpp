#ifndef SINGLELINKLIST_H
#define SINGLELINKLIST_H

#include<mge/util/list/SingleLinkNode.hpp>
#include<cstddef>

namespace Utils
{
    template<class T>
    class SingleLinkList
    {
    public:
        SingleLinkNode<T>* startNode = NULL;

        void Add(SingleLinkNode<T>* target)
        {
            if(target==NULL)
                return;

            target->nextNode = startNode;
            startNode = target;
        }
    };
}

#endif // SINGLELINKLIST_H
