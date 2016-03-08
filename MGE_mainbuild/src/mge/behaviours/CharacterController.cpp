#include "CharacterController.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/World.hpp"

CharacterController::CharacterController(float moveSpeed, float jumpStrength, float gravity, float colliderRadius, float jumpDuration)
{
    _name = "Character Controller";
    _moveSpeed = moveSpeed;
    _jumpForce = jumpStrength;
    _gravityForce = gravity;
    _colliderRadius = colliderRadius;
    //_wasOnGround = false;
    _isOnGround = false;
    jumping = false;

    _jumpTimer.SetDuration(jumpDuration);
    _jumpTimer.Reset();
}

void CharacterController::Update()
{
    Movement();

    //if was on ground the previous frame, handle jump
    if(Input::GetKey(Input::Space)&& _isOnGround)
    {
        _jumpTimer.Reset();
        jumping = true;
    }

    if(Input::GetKey(Input::L))
    {
        _jumpTimer.Reset();
        jumping = true;
    }

    if(jumping)
        HandleJump();

    _isOnGround = false;
}

void CharacterController::Movement()
{
	float moveX = 0.0f;
	float moveZ = 0.0f;

	//float jump = 0.0f;

	if ( Input::GetKey(Input::W)) {
		moveZ += _moveSpeed;
	}
	if ( Input::GetKey(Input::S)) {
		moveZ -= _moveSpeed;
	}
	if ( Input::GetKey(Input::D)) {
		moveX -= _moveSpeed;
	}
	if ( Input::GetKey(Input::A)) {
		moveX += _moveSpeed;
	}

    /*
	if ( Input::GetKey(Input::Space)) {
		jump += _jumpForce;
	}
    else
        jump -= _gravityForce;
        */

	glm::mat4 temp = World::getMainCamera()->getWorldTransform();

	glm::vec3 forward = -glm::vec3(temp[2]);
	glm::vec3 right =  glm::vec3(temp[0]);
    glm::vec3 forward1 = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
    glm::vec3 right1 = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

    glm::mat4 myMat = _owner->getWorldTransform();
	myMat[3] += glm::vec4(right1 * moveX,0);
	myMat[3] += glm::vec4(forward1 * moveZ,0);


    myMat[3] += glm::vec4(0,-_gravityForce,0,0);
	_owner->setWorldTransform(myMat);
	//_owner->translate(glm::vec3(0,2,0));

	//std::cout<<_owner->getWorldPosition()<<"\n";
}

void CharacterController::setOwner(GameObject* owner)
{
    Component::setOwner(owner);
    _collider = new SphereCollider();
    _collider->radius = _colliderRadius;
    _collider->SetStatic(false);
    _owner->AttachComponent(_collider);
    _collider->OnCollision = [this](Collider* other, CollisionMTV mtv){ CollisionCallback(other, mtv);};
}

void CharacterController::CollisionCallback(Collider* other, CollisionMTV mtv)
{
    if(other->getOwner()->getName().find("floor")!=std::string::npos)
    {
        float angle = glm::angle(mtv.axis,glm::vec3(0,1,0));
        if(angle <= 0.786f)
        {
            _isOnGround = true;

            using namespace glm;//because fuck writing glm:: all the time

            vec3 displacement = mtv.axis * mtv.magnitude;
            vec3 ownerPos = _owner->getWorldPosition();
            vec3 otherToMe = ownerPos-other->getOwner()->getWorldPosition();
            if(dot(displacement,otherToMe)<0.0f)
                displacement = displacement * -1.0f;

            _owner->setWorldPosition(ownerPos - vec3(displacement.x,0,displacement.z)*1.01f); //1% more to counter the wall collider's 1% extra push
        }



    }

}

void CharacterController::HandleJump()
{
    if(_jumpTimer.isFinished())
    {
        jumping = false;
        return;
    }

    _owner->setWorldPosition(_owner->getWorldPosition() + glm::vec3(0,_jumpForce,0));

}

CharacterController::~CharacterController()
{

}
