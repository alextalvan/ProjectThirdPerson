#ifndef TEST_SCOL
#define TEST_SCOL
#include<mge/collision/SphereCollider.hpp>
#include<mge/collision/BoxCollider.hpp>
#include<mge/collision/Collider.hpp>

namespace Test
{
    class TestSphereCol : public SphereCollider
    {
    public:
        TestSphereCol();

        void Update() override;

    private:
        void CollisionCallback(Collider* other, CollisionMTV mtv);
    };
}

#endif // TEST_SCOL
