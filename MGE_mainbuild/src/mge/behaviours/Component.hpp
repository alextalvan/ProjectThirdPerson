#ifndef ABSTRACTBEHAVIOUR_H
#define ABSTRACTBEHAVIOUR_H
#include "mge/core/Activateable.hpp"
#include "mge/core/Destroyable.hpp"
#include "mge/util/list/DualLinkList.hpp"
#include "mge/util/list/DualLinkNode.hpp"
#include <string>

class GameObject;

/**
 *Component, just like Unity.
 */
class Component: public Activateable, public Destroyable, public DualLinkNode<Component>
{
	public:
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
