#pragma once
#include <mge/behaviours/Component.hpp>

class FirstPersonLook : public Component
{
public:
    FirstPersonLook(float turnSpeed = 1.0f);

protected:
    virtual ~FirstPersonLook();
    virtual void Update() override;


private:
        void Rotation();
        glm::vec2 GetMouseOffset();
        glm::vec2 prevMousePos;
        float rotationX = 0.0f;
        float rotationY = 0.0f;
        float _turnSpeed;
};
