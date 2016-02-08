#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <string>
#include <iostream>
#include <glm.hpp>
#include "mge/core/Activateable.hpp"
#include "mge/core/Destroyable.hpp"
#include "mge/core/AbstractGame.hpp"

#include "lua.hpp"
#include "luabridge.h" //new include for LuaBridge

class AbstractCollider;
class Component;
class AbstractMaterial;
class World;
class Mesh;
//class Activateable;

class GameObject : public Activateable, public Destroyable
{
    friend class AbstractGame;

	public:
		GameObject(std::string pName = NULL, glm::vec3 pPosition = glm::vec3( 0.0f, 0.0f, 0.0f ) );
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

		void AttachComponent(Component* comp);

		void DetachComponent(Component* comp);

		void DestroyComponent(Component* comp);

        int GetComponentsCount();
        Component* GetComponentAt (int pIndex);

        //note that this thing returns the FIRST component of type tp found, if it exists
        template<class tp>
        tp* GetComponent()
        {
            for(int i = 0; i< GetComponentsCount(); ++i)
            {
                tp* ret = dynamic_cast<tp*>(GetComponentAt(i));
                if(ret)
                    return ret;
            }
            return NULL;
        }




        //child management

        //shortcut to set the parent of pChild to ourselves
		void AddChild (GameObject* pChild);
		//shortcut to set the parent of pChild to NULL
		void RemoveChild (GameObject* pChild);

		virtual void setParent (GameObject* pGameObject);
		GameObject* getParent();

        int GetChildCount();
        GameObject* GetChildAt (int pIndex);

		static void register_lua(lua_State* L)
		{
			using namespace luabridge;
			getGlobalNamespace(L) //global namespace to lua
				.beginNamespace("Engine") //our defined namespace (w.e we want to call it)
				.beginClass<GameObject>("GameObject") //define class object
				.addConstructor<void(*)(std::string)>() //reg. empty constructor
				.addFunction("setName", &GameObject::setName) //reg. setName function
				.addFunction("getName", &GameObject::getName) //reg. setAge function
				.addFunction("setLocalPosition", &GameObject::setLocalPosition) //reg. setLocalPosition function
				.addFunction("getLocalPosition", &GameObject::getLocalPosition) //reg. getLocalPosition function
				.addFunction("getWorldPosition", &GameObject::getWorldPosition) //reg. getWorldPosition function
				.addFunction("translate", &GameObject::translate) //reg. translate function
				.addFunction("rotate", &GameObject::rotate) //reg. rotate function
				.addFunction("scale", &GameObject::scale) //reg. scale function
				.addFunction("setMesh", &GameObject::setMesh) //reg. setMesh function
				.addFunction("getMesh", &GameObject::getMesh) //reg. getMesh function
				.addFunction("setMaterial", &GameObject::setMaterial) //reg. setMaterial function
				.addFunction("getMaterial", &GameObject::getMaterial) //reg. getMaterial function
				.addFunction("AddChild", &GameObject::AddChild) //reg. AddChild function
				.addFunction("RemoveChild", &GameObject::RemoveChild) //reg. RemoveChild function
				.addFunction("setParent", &GameObject::setParent) //reg. setParent function
				.addFunction("getParent", &GameObject::getParent) //reg. getParent function
				.addFunction("GetChildCount", &GameObject::GetChildCount) //reg. GetChildCount function
				.addFunction("GetChildAt", &GameObject::GetChildAt) //reg. getParent function
				.addFunction("Destroy", &GameObject::Destroy) //reg. getParent function
				.endClass() //end class
				.endNamespace(); //end namespace
		}

	public:
	    virtual ~GameObject();
		std::string _name;
		glm::mat4 _transform;
		glm::mat4 _worldTransform;

        GameObject* _parent;
		std::vector<GameObject*> _children;

        Mesh* _mesh;

        std::vector<Component*> _components;
		//Component* _behaviour;
		AbstractMaterial* _material;
		World* _world;

        //update children list administration
        void _innerAdd (GameObject* pChild);
		void _innerRemove (GameObject* pChild);

		virtual void Update();


    private:
        bool _worldTransformIsDirty = true;
        void MakeTransformDirty();
        void InternalUpdate();
        //bool _transformIsDirty = true;
};

#endif // GAMEOBJECT_H
