#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H
#include <mge/collision/Collider.hpp>
#include <mge/collision/SphereCollider.hpp>

class BoxCollider : public Collider
{
friend class SphereCollider;
public:
    float xSize = 2.0f;
    float ySize = 2.0f;
    float zSize = 2.0f;
    BoxCollider();

protected:
    virtual ~BoxCollider();
    bool HitTest(SphereCollider* other) override;
    bool HitTest(BoxCollider* other) override;
    bool HitTest(Collider* other) override;
    bool RayTest(const Ray& ray, float& distance) override;

private:
    bool InternalHitTest(SphereCollider* other);
};

#endif // BOXCOLLIDER_H
