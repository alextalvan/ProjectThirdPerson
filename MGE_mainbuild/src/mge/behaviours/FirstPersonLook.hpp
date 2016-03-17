#pragma once
#include <mge/behaviours/Component.hpp>
#include <mge/collision/CollisionManager.hpp>

class FirstPersonLook : public Component
{
public:
    FirstPersonLook(float turnSpeed = 1.0f);
    void SetActive(bool state) override;

    static const RaycastInfo& GetLastRaycastInfo();

protected:
    virtual ~FirstPersonLook();
    virtual void Update() override;


private:
        void Rotation();
        glm::vec2 GetMouseOffset();
        glm::vec2 prevMousePos;
        float rotationX = 180.0f;
        float rotationY = 0.0f;
        float _turnSpeed;

        static RaycastInfo _storedRaycastInfo;
};
