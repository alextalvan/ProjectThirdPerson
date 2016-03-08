#include <mge/collision/TerrainCollider.hpp>
#include <iostream>
#include <mge/collision/SphereCollider.hpp>
#include <mge/config.hpp>

TerrainCollider::TerrainCollider(std::string heightMapName, float sizeX, float sizeZ, float height)
{
    _heightMap.loadFromFile(config::MGE_TEXTURE_PATH + heightMapName);
    _xSize = sizeX;
    _zSize = sizeZ;
    _height = height;

    _name = "Terrain Collider";
    layer = CollisionManager::WALLS;
    SetStatic(true);

    OnCollision = [this](Collider* other, CollisionMTV& mtv) { CollisionCallback(other, mtv);};
}

TerrainCollider::~TerrainCollider()
{

}

bool TerrainCollider::HitTest(SphereCollider* other)
{
    return InternalHitTest(other);
}

bool TerrainCollider::InternalHitTest(SphereCollider* other)
{
    using namespace glm;
    glm::vec3 sphereCenter = other->GetBoundingSphere().position;
    float sphereRadius = other->GetBoundingSphere().radius;

    vec3 terrainPos = _bound.position;

    //AABB check
    if(sphereCenter.x - sphereRadius < terrainPos.x - _xSize * 0.5f)
        return false;

    if(sphereCenter.x + sphereRadius > terrainPos.x + _xSize * 0.5f)
        return false;

    if(sphereCenter.z - sphereRadius < terrainPos.z - _zSize * 0.5f)
        return false;

    if(sphereCenter.z - sphereRadius > terrainPos.z + _zSize * 0.5f)
        return false;


    vec3 relativePos = sphereCenter - terrainPos;
    float terrainHeight = SampleHeightAt(relativePos);

    //std::cout<<sphereCenter.z - sphereRadius<<" "<<terrainHeight<<"\n";

    //std::cout<<sphereCenter<<"\n";

    float penetrationDepth = terrainHeight - (sphereCenter.y - sphereRadius);

    if(penetrationDepth > 0)
    {
        //std::cout<<(sphereCenter.y - sphereRadius)<<"\n";
        Collider::storedMTV.magnitude = penetrationDepth;
        Collider::storedMTV.axis = glm::vec3(0,1,0);
        return true;
    }


    return false;
}

bool TerrainCollider::HitTest(BoxCollider* other)
{
    return false;
}


float TerrainCollider::SampleHeightAt(glm::vec3 relativePos)
{
    using namespace glm;
    vec2 uv = vec2((relativePos.x / _xSize + 0.5f ),1.0f - (relativePos.z / _zSize + 0.5f ));

    //if(uv.x > 1.0f || uv.x <0.0f || uv.y > 1.0f || uv.y < 0.0f)
    //    throw "TerrainCollider uv check out of bounds";

    uv = glm::saturate(uv);

    //std::cout<<uv<<" ";

    uv.x *= _heightMap.getSize().x;
    uv.y *= _heightMap.getSize().y;

    //std::cout<<uv<<"\n";

    int discreteX = glm::floor(uv.x);
    int discreteZ = glm::floor(uv.y);

    float fractionalX = glm::fract(uv.x);
    float fractionalZ = glm::fract(uv.y);

    float topLeft =  (_heightMap.getPixel(discreteX,discreteZ)).r;
    float topRight = (_heightMap.getPixel(discreteX,discreteZ + 1)).r;
    float bottomLeft = (_heightMap.getPixel(discreteX+1,discreteZ)).r;
    float bottomRight = (_heightMap.getPixel(discreteX+1,discreteZ+1)).r;

    float topBlend = glm::lerp(topLeft,topRight,fractionalX);
    float botBlend = glm::lerp(bottomLeft,bottomRight,fractionalX);



    float ret = glm::lerp(topBlend,botBlend,fractionalZ) * 0.00390625f * _height;//magic value is 1/256, convert from int rgb to float 0..1
    //std::cout<<ret<<" "<<botBlend<<"\n";
    return ret;


    //int sample
    //vec3 baseColor =
    //return (_heightMap.getPixel(uv.x,uv.y)).r;
}


void TerrainCollider::CollisionCallback(Collider* other, CollisionMTV& mtv)
{
    GameObject* otherOwner = other->getOwner();

    otherOwner->setWorldPosition(otherOwner->getWorldPosition() + glm::vec3(0,1,0) * mtv.magnitude);

}


bool TerrainCollider::HitTest(TerrainCollider* other)
{
    return false;//lmao no
}

bool TerrainCollider::HitTest(Collider* other)
{
    if(_owner == NULL || other->getOwner()==NULL)
        return false;

    return other->HitTest(this);
}

bool TerrainCollider::RayTest(const Ray& ray, float& distance)
{
    return false;
}

void TerrainCollider::RefreshBoundingSphere()
{
    if(_owner==NULL)
        return;

    glm::mat4 objMat = _owner->getWorldTransform();
    _bound.position = glm::vec3(objMat[3]);

    float xRad = glm::length(objMat[0] * _xSize * 0.5f);
    float yRad = glm::length(objMat[1] * _height);
    float zRad = glm::length(objMat[2] * _zSize * 0.5f);

    _bound.radius = max(xRad,max(yRad,zRad));
}
