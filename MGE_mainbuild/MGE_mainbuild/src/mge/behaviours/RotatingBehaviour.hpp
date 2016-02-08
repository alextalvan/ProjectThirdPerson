#ifndef ROTATINGBEHAVIOUR_H
#define ROTATINGBEHAVIOUR_H

#include "mge/behaviours/Component.hpp"

/**
 * Simply rotates the object around its origin
 */
class RotatingBehaviour : public Component
{
	public:
		RotatingBehaviour();
		virtual ~RotatingBehaviour();

		virtual void Update();

};

#endif // ROTATINGBEHAVIOUR_H
