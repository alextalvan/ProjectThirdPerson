#ifndef LookAt_H
#define LookAt_H

#include <glm.hpp>
#include "mge/behaviours/Component.hpp"

/**
 * Keeps the owner looking at the given target.
 */
class LookAt : public Component
{
    public:
        LookAt (GameObject * pTarget);
        virtual ~LookAt();

        virtual void Update();

    private:
        GameObject * _target;   //what are we looking at?
};

#endif // LookAt_H
