#pragma once

#include <mge/collision/Collider.hpp>
#include <string>
#include <SFML/Graphics.hpp>
#include "glm.hpp"

class TerrainCollider : public Collider
{
friend class SphereCollider;
public:
    TerrainCollider(std::string heightMapName, float sizeX, float sizeZ, float height);

protected:
    bool HitTest(SphereCollider* other) override;
    bool HitTest(BoxCollider* other) override;
    bool HitTest(Collider* other) override;
    bool HitTest(TerrainCollider* other) override;
    bool RayTest(const Ray& ray, float& distance) override;
    void RefreshBoundingSphere() override;

    virtual ~TerrainCollider();

private:
    float _xSize, _zSize, _height;
    sf::Image _heightMap;

    void CollisionCallback(Collider* other, CollisionMTV& mtv);

    float SampleHeightAt(glm::vec3 relativePos);

    bool InternalHitTest(SphereCollider* other);
};

