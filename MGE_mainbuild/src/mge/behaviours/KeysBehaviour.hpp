#ifndef KEYSBEHAVIOUR_H
#define KEYSBEHAVIOUR_H

#include "mge/behaviours/Component.hpp"

/**
 * KeysBehaviour allows you to move an object using the keyboard in its own local space.
 * Left right turns, forward back moves.
 */
class KeysBehaviour : public Component
{
	public:
		KeysBehaviour(float moveSpeed = 0.1f, float turnSpeed = 1);
		virtual ~KeysBehaviour();
		virtual void Update();

    private:
        float _moveSpeed;
        float _turnSpeed;
};

#endif // KEYSBEHAVIOUR_H
