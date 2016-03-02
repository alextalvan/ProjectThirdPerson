#include "mge/collision/QuadTreeUtil.hpp"

namespace QuadTreeUtil
{
    QuadAABB::QuadAABB(float plusX, float minusX,float plusZ, float minusZ)
    {
        _plusX = plusX;
        _minusX = minusX;
        _plusZ = plusZ;
        _minusZ = minusZ;
    }

    bool QuadAABB::OverlapCheck(const ColliderBoundingSphere& boundSphere)
    {
        if(boundSphere.position.z + boundSphere.radius < _minusZ )
            return false;

        if(boundSphere.position.z - boundSphere.radius > _plusZ )
            return false;

        if(boundSphere.position.x + boundSphere.radius < _minusX )
            return false;

        if(boundSphere.position.x - boundSphere.radius > _plusX )
            return false;

        return true;
    }

    glm::vec4 QuadAABB::GetBounds()
    {
        return glm::vec4(_plusX,_minusX,_plusZ,_minusZ);
    }

}
