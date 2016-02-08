#ifndef CAMERAORBIT_H
#define CAMERAORBIT_H

#include <mge/behaviours/Component.hpp>
namespace Test
{
    class CameraOrbit : public Component
    {
    public:
        CameraOrbit();
        float horizontalDistance = 2.0f;
        float verticalDistance = 2.0f;

        float horizRotationSpeed = 0.1f;
        float vertRotationSpeed = 0.1f;
        float maxVertAngle = 30.0f;

        GameObject* target = NULL;

    protected:
        virtual ~CameraOrbit();
        void Update() override;

    private:
        int prevMouseX;
        int prevMouseY;

        float currentHorizAngle = 0.0f;
        float currentVertAngle = 0.0f;
    };
}

#endif // CAMERAORBIT_H
