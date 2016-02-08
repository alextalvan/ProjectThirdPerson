#ifndef SPHERECOLLIDER_H
#define SPHERECOLLIDER_H

#include<mge/collision/Collider.hpp>


class SphereCollider : public Collider
{
   public:
       SphereCollider();
       float radius = 1.0f;

   protected:
    virtual ~SphereCollider();
    bool HitTest(SphereCollider* other) override;
    bool HitTest(BoxCollider* other) override;
    bool HitTest(Collider* other) override;
};

#endif // SPHERECOLLIDER_H
