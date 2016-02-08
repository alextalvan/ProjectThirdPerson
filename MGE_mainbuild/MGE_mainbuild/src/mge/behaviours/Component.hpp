#ifndef ABSTRACTBEHAVIOUR_H
#define ABSTRACTBEHAVIOUR_H
#include "mge/core/Activateable.hpp"
#include "mge/core/Destroyable.hpp"
#include <string>

class GameObject;

/**
 * class Behaviour allows you to attach reusable behaviours to a GameObject.
 * Think of steering, rotating, billboarding, etc.
 * A Behaviour is set on a GameObject, which in turn passes in a reference to itself through the setOwner method.
 * This way we can enforce that a Behaviour can never have an owner different from the object it has been attached to.
 *
 * It is similar to MonoBehaviour in Unity.
 */
class Component: public Activateable, public Destroyable
{
	public:



        //we would like to have this private and only accessible by GameObject, but this
        //doesnt work out for the CompositeBehaviour, we would have to declare both of them
        //as friends, tying this class to one of its subclasses, so design decision:
        //this is kept public but should not be used directly.
        virtual void setOwner (GameObject* pGameObject);

        GameObject* const getOwner() const;

		virtual void Update();

        virtual void Destroy() override;

        std::string getName() const;

    protected:
        Component();
        std::string _name = "Unnamed Component";
        virtual ~Component();
	    //reference back its owner
		GameObject* _owner;

};

#endif // ABSTRACTBEHAVIOUR_H
