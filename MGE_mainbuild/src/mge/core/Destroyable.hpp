#ifndef DESTROYABLE_H
#define DESTROYABLE_H
#include <mge/util/list/SingleLinkList.hpp>
using namespace Utils;

class Destroyable : private SingleLinkNode<Destroyable>
{
friend class AbstractGame;
public:
    virtual void Destroy();
    bool isDestroyed();

protected:
    Destroyable();
    virtual ~Destroyable();

private:
    bool _toBeDestroyed = false;

    //central GC
    static SingleLinkList<Destroyable> _objectList;
    static void CollectGarbage();
};

#endif // DESTROYABLE_H
