#ifndef DUAL_LINK_NODE_H
#define DUAL_LINK_NODE_H
#include <cstddef>

namespace Utils
{
    template<class T>
    class DualLinkNode
    {
    public:
        DualLinkNode<T>* nextNode = NULL;
        DualLinkNode<T>* prevNode = NULL;
    };

    template<class T>
    class DualLinkNode2
    {
    public:
        DualLinkNode2<T>* nextNode = NULL;
        DualLinkNode2<T>* prevNode = NULL;
    };

}


#endif // DUAL_LINK_NODE_H
