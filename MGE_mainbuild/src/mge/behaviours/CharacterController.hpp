#ifndef CHARACTERCONTROLLER_H
#define CHARACTERCONTROLLER_H

#include <mge/behaviours/Component.hpp>
#include <glm.hpp>
#include "mge/util/Input.hpp"

class GameObject;

class CharacterController : public Component
{
    public:
        CharacterController(float moveSpeed = 0.2f, float jumpStrength = 1.0f, float gravity = 0.1f);

		void Update() override;
    protected:
        virtual ~CharacterController();
    private:
        void Movement();

        float _jumpForce;
        float _gravityForce;
        float _moveSpeed;
};

#endif // CHARACTERCONTROLLER_H
