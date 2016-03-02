#ifndef SPHERECOLLIDER_H
#define SPHERECOLLIDER_H

#include <mge/collision/Collider.hpp>
#include <mge/util/list/DualLinkNode.hpp>
#include <mge/collision/CollisionManager.hpp>


class SphereCollider : public Collider
{
   public:
       SphereCollider(float r = 1.0f, bool ignorRay = true);
       float radius;

   protected:
        virtual ~SphereCollider();
        bool HitTest(SphereCollider* other) override;
        bool HitTest(BoxCollider* other) override;
        bool HitTest(Collider* other) override;
        bool RayTest(const Ray& ray, float& distance) override;
        void RefreshBoundingSphere() override;

};

#endif // SPHERECOLLIDER_H
