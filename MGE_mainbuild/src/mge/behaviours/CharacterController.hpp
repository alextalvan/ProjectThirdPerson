#ifndef CHARACTERCONTROLLER_H
#define CHARACTERCONTROLLER_H

#include <mge/behaviours/KeysBehaviour.hpp>
#include <glm.hpp>
#include "mge/util/Input.hpp"

class GameObject;

class CharacterController : public KeysBehaviour
{
    public:
        CharacterController(float moveSpeed = 1.0f, float turnSpeed = 1.0f);
        virtual ~CharacterController();
		virtual void Update();
    protected:
    private:
        void Movement();
        void Rotation();
        glm::vec2 GetMouseOffset();
        glm::vec2 prevMousePos;
        float rotationX = 0.0f;
        float rotationY = 0.0f;
};

#endif // CHARACTERCONTROLLER_H
