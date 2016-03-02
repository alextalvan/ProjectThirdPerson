#ifndef WALLCOLLIDER_H
#define WALLCOLLIDER_H

#include "mge/collision/BoxCollider.hpp"

class WallCollider : public BoxCollider
{
public:
    WallCollider();
    bool ignoreXaxis = false;
    bool ignoreYaxis = false;
    bool ignoreZaxis = false;
protected:
    virtual ~WallCollider();
private:
    void CollisionCallback(Collider* other, CollisionMTV& mtv);
};

#endif // WALLCOLLIDER_H
