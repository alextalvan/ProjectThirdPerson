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
#include "mge/lua/LuaObject.hpp"

class AbstractCollider;
class Component;
class AbstractMaterial;
class World;
class Mesh;
//class Activateable;

class ChildList;
//class TransparencyList;

class GameObject : public Activateable, public Destroyable, public DualLinkNode2<ChildList>, public LuaObject
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

        void setLocalRotation(glm::vec3 forward);
        void setLocalRotation(glm::vec3 xAxis, glm::vec3 yAxis,glm::vec3 zAxis);
        void setWorldRotation(glm::vec3 forward);
        void setWorldRotation(glm::vec3 xAxis, glm::vec3 yAxis,glm::vec3 zAxis);
        //get the objects world position
		glm::vec3 getWorldPosition();
		void setWorldPosition(glm::vec3 pos);


		glm::vec3 getLocalScale();
		void setLocalScale(glm::vec3 pos);

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

		virtual void AttachComponent(Component* comp);

		virtual void DetachComponent(Component* comp);

		virtual void DestroyComponent(Component* comp);

		virtual void DestroyComponents();

        int GetComponentsCount();
        Component* GetComponentAt (int pIndex);
        Component* FindComponent(std::string name);
        DualLinkList<Component>& GetComponents();

        //void setTransparent(bool val);
        //bool isTransparent();

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

        DualLinkList2<ChildList>& GetChildren();

        GameObject* FindChild(std::string name);

        bool castShadows = true;

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
		virtual void MakeTransformDirty();
		virtual void _recalculateLocalTransform();


    private:
        bool _worldTransformIsDirty = true;

        void MakeChildrenTransformsDirty();

        void InternalUpdate();

         //update children list administration
        void _innerAdd (GameObject* pChild);
		void _innerRemove (GameObject* pChild);

        void _innerAdd (Component* comp);
		void _innerRemove (Component* comp);
        //bool _transformIsDirty = true;
};

#endif // GAMEOBJECT_H
