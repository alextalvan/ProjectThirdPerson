#ifndef ABSTRACTBEHAVIOUR_H
#define ABSTRACTBEHAVIOUR_H
#include "mge/core/Activateable.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/core/Destroyable.hpp"
#include "mge/util/list/DualLinkList.hpp"
#include "mge/util/list/DualLinkNode.hpp"
#include "mge/lua/LuaObject.hpp"
#include <string>

class GameObject;

/**
 *Component, just like Unity.
 */
class Component: public Activateable, public Destroyable, public DualLinkNode<Component>, public LuaObject
{
    friend class GameObject;
	public:
        GameObject* const getOwner() const;

        void Destroy() override;

        std::string getName() const;

    protected:
        virtual void Update();

        Component();
        virtual void setOwner (GameObject* owner);
        std::string _name = "Unnamed Component";
        virtual ~Component();
	    //reference back its owner
    //private:
		GameObject* _owner;

};

#endif // ABSTRACTBEHAVIOUR_H
