#ifndef CHARACTERCONTROLLER_H
#define CHARACTERCONTROLLER_H

#include <mge/behaviours/Component.hpp>
#include <glm.hpp>
#include "mge/util/Input.hpp"
#include "mge/collision/SphereCollider.hpp"
#include "mge/util/Timer.hpp"

class GameObject;

class CharacterController : public Component
{
    public:
        CharacterController(float moveSpeed = 0.2f, float jumpStrength = 1.0f, float gravity = 0.1f, float colliderRadius = 1.0f, float jumpDuration = 0.25f);

		void Update() override;
    protected:
        virtual ~CharacterController();
        virtual void setOwner(GameObject* owner) override;
    private:
        void Movement();
        void HandleJump();
        void CollisionCallback(Collider* other, CollisionMTV mtv);

        float _jumpForce;
        float _gravityForce;
        float _moveSpeed;
        float _colliderRadius;
        SphereCollider* _collider;

        bool jumping, _isOnGround;

        Timer _jumpTimer;
};

#endif // CHARACTERCONTROLLER_H
