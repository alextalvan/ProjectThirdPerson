#ifndef SINGLELINKNODE_H
#define SINGLELINKNODE_H
#include<cstddef>

namespace Utils
{
    template<class T>
    class SingleLinkNode
    {
    public:
        SingleLinkNode<T>* nextNode = NULL;

    protected:
        SingleLinkNode<T>()
        {

        };
    };
}


#endif // SINGLELINKNODE_H
