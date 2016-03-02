#pragma once
#include "glm.hpp"
#include "mge/collision/Collider.hpp"

namespace QuadTreeUtil
{
    class QuadAABB
    {
    public:
        QuadAABB(float plusX, float minusX,float plusZ, float minusZ);
        bool OverlapCheck(const ColliderBoundingSphere& boundSphere);
        glm::vec4 GetBounds();
    private:
        float _plusZ, _minusZ, _plusX, _minusX;
    };
}
