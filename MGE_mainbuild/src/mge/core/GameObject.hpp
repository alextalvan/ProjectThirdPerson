#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <string>
#include <iostream>
#include <glm.hpp>
#include "mge/core/Activateable.hpp"
#include "mge/core/Destroyable.hpp"
#include "mge/core/AbstractGame.hpp"
#include "mge/util/list/DualLinkList.hpp"
#include "mge/util/list/DualLinkNode.hpp"

class AbstractCollider;
class Component;
class AbstractMaterial;
class World;
class Mesh;
//class Activateable;

class ChildList;

class GameObject : public Activateable, public Destroyable, public DualLinkNode2<ChildList>
{
    friend class AbstractGame;
	public:
		GameObject(std::string pName = "GameObject", glm::vec3 pPosition = glm::vec3( 0.0f, 0.0f, 0.0f ) );
		//GameObject(std::string pName = NULL, float posX = 0.0f, float posY = 0.0f, float posZ = 0.0f);

        virtual void Destroy() override;

        void setName (std::string pName);
        std::string getName() const;

        //contains local rotation, scale, position
		void setTransform (const glm::mat4& pTransform);
        glm::mat4& getTransform();

        //access just the local position
		void setLocalPosition (glm::vec3 pPosition);
		glm::vec3 getLocalPosition();

        //get the objects world position by combining transforms
		glm::vec3 getWorldPosition();
		virtual glm::mat4& getWorldTransform();
		void setWorldTransform(const glm::mat4& pTransform);

        //change local position, rotation, scaling
		void translate(glm::vec3 pTranslation);
		void rotate(float pAngle, glm::vec3 pAxis);
		void scale(glm::vec3 pScale);

        //mesh and material should be shared as much as possible
		void setMesh(Mesh* pMesh);
		Mesh* getMesh() const;

		void setMaterial (AbstractMaterial* pMaterial);
		AbstractMaterial* getMaterial() const;

        //behaviour will probably be a unique instance per gameobject
        //NOPE
		//void setBehaviour(Component* pBehaviour);
		//Component* getBehaviour() const;

		virtual void AttachComponent(Component* comp);

		virtual void DetachComponent(Component* comp);

		virtual void DestroyComponent(Component* comp);

		virtual void DestroyComponents();

        int GetComponentsCount();
        Component* GetComponentAt (int pIndex);

        //note that this thing returns the FIRST component of type tp found, if it exists
        template<class T>
        T* GetComponent()
        {
            DualLinkNode<Component>* cn = _components.startNode;
            while(cn!=NULL)
            {
                T* ret = dynamic_cast<T*>((Component*)cn);
                if(ret)
                    return ret;
                cn = cn->nextNode;
            }
            return NULL;
        }

        //child management

        //shortcut to set the parent of pChild to ourselves
		void AddChild (GameObject* pChild);
		//shortcut to set the parent of pChild to NULL
		void RemoveChild (GameObject* pChild);

		void DestroyChildren();

		virtual void setParent (GameObject* pGameObject);
		GameObject* getParent();

        int GetChildCount();
        GameObject* GetChildAt (int pIndex);


	protected:
	    virtual ~GameObject();
		std::string _name;
		glm::mat4 _transform;
		glm::mat4 _worldTransform;

        GameObject* _parent;
		//std::vector<GameObject*> _children;
		DualLinkList2<ChildList> _children;

        Mesh* _mesh;

        DualLinkList<Component> _components;
		//Component* _behaviour;
		AbstractMaterial* _material;
		//World* _world;



		virtual void Update();


    private:
        bool _worldTransformIsDirty = true;
        void MakeTransformDirty();
        void InternalUpdate();

         //update children list administration
        void _innerAdd (GameObject* pChild);
		void _innerRemove (GameObject* pChild);

        void _innerAdd (Component* comp);
		void _innerRemove (Component* comp);
        //bool _transformIsDirty = true;
};

#endif // GAMEOBJECT_H
