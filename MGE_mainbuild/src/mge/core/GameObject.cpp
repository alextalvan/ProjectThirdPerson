#include <cassert>
#include <iostream>
using namespace std;

#include <glm.hpp>

#include "mge/core/GameObject.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/World.hpp"
#include "mge/behaviours/Component.hpp"

GameObject::GameObject(std::string pName, glm::vec3 pPosition )
:	_name( pName ), _transform( glm::translate( pPosition ) ),  _parent(NULL), _children(),
    _mesh( NULL ),_components(), _material(NULL)//,_world(NULL)
{
}

/*
GameObject::GameObject(std::string pName, float posX, float posY, float posZ)
	: _name(pName), _transform(glm::translate(glm::vec3(posX, posY, posZ))), _parent(NULL), _children(),
	_mesh(NULL), _behaviour(NULL), _material(NULL), _world(NULL)
{
}
*/

GameObject::~GameObject()
{
    //detach all children
    cout << getName()<<" was released.\n";
}

void GameObject::Destroy()
{

    Destroyable::Destroy();
    SetActive(false);

    setParent(NULL);

    //destroy all my components
    DestroyComponents();

    //destroy children as well
    DestroyChildren();
}

void GameObject::DestroyChildren()
{
    DualLinkNode2<ChildList>* cn2 = _children.startNode;

    while(cn2!=NULL)
    {
        ((GameObject*)cn2)->Destroy();
        cn2 = cn2->nextNode;
    }

    //test fix
    //_children.Clear();
}

void GameObject::DestroyComponents()
{
    DualLinkNode<Component>* cn = _components.startNode;
    while(cn!=NULL)
    {
        ((Component*)cn)->Destroy();
        cn = cn->nextNode;
    }
}

void GameObject::setName (std::string pName)
{
    _name = pName;
}

std::string GameObject::getName() const
{
	return _name;
}

void GameObject::setTransform (const glm::mat4& pTransform)
{
    _transform = pTransform;
    MakeTransformDirty();
}

glm::mat4& GameObject::getTransform()
{
    return _transform;
}

void GameObject::setLocalPosition (glm::vec3 pPosition)
{
    _transform[3] = glm::vec4 (pPosition,1);
    MakeTransformDirty();
}

glm::vec3 GameObject::getLocalPosition()
{
	return glm::vec3(_transform[3]);
}

void GameObject::setMaterial(AbstractMaterial* pMaterial)
{
    _material = pMaterial;
}

AbstractMaterial * GameObject::getMaterial() const
{
    return _material;
}

void GameObject::setMesh(Mesh* pMesh)
{
	_mesh = pMesh;
}

Mesh * GameObject::getMesh() const
{
    return _mesh;
}

void GameObject::_innerAdd(Component* comp)
{
    _components.Add(comp);
}

void GameObject::_innerRemove(Component* comp)
{
    _components.Remove(comp);
}

void GameObject::AttachComponent(Component* comp)
{
    if(comp==NULL||comp->_owner==this)
        return;

    if(comp->_owner!=NULL)//detach from prev owner, if any
        comp->_owner->_innerRemove(comp);//we already know its owner has it so no need to use Detach() which does a safety check

    _innerAdd(comp);
    comp->setOwner(this);//this is used because derived Components may want to have special behaviour when attached to objects
}

void GameObject::DetachComponent(Component* comp)
{
    if(comp==NULL)
        return;

    //safety check in case the list does not actually contain the component
    if(_components.Contains(comp))
    {
        _innerRemove(comp);
        comp->setOwner(NULL);
    }

}

void GameObject::DestroyComponent(Component* comp)
{
    DetachComponent(comp);
    comp->Destroy();
}

void GameObject::setParent (GameObject* pParent) {
    //remove from previous parent
    if (_parent != NULL) {
        _parent->_innerRemove(this);
        _parent = NULL;
        //_world = NULL;
    }

    //set new parent
    if (pParent != NULL) {
        _parent = pParent;
        _parent->_innerAdd(this);
        //pass on world to child
        //_world = _parent->_world;
    }

    MakeTransformDirty();
}

GameObject* GameObject::getParent() {
    return _parent;
}

void GameObject::_innerAdd(GameObject* pChild)
{
    //set new parent
    pChild->_parent = this;
    _children.Add(pChild);
	//_children.push_back(pChild);
}

void GameObject::_innerRemove (GameObject* pChild)
{
    /*
    DualLinkNode2<ChildList>* cn = _children.startNode;

    while(cn!=NULL)
    {
       GameObject* cast = (GameObject*)cn;
       if(cast == pChild)
       {
            cast->_parent = NULL;
            _children.Remove(cn);
            return;
       }
        cn = cn->nextNode;
    }
    */

    if(_children.SafeRemove(pChild))
        pChild->_parent = NULL;

}

GameObject* GameObject::FindChild(std::string name)
{
    DualLinkNode2<ChildList>* cn2 = _children.startNode;

    while(cn2!=NULL)
    {
        GameObject* obj = (GameObject*)cn2;
        if(obj->getName()==name)
            return obj;
        cn2 = cn2->nextNode;
    }

    return NULL;
}

void GameObject::AddChild (GameObject* pChild) {
    pChild->setParent(this);
}

void GameObject::RemoveChild (GameObject* pChild) {
    pChild->setParent(NULL);
}

void GameObject::setLocalRotation(glm::vec3 xAxis, glm::vec3 yAxis,glm::vec3 zAxis)
{
    using namespace glm;
    //extract scaling from the current matrix
    vec3 scale = vec3(length(_transform[0]),length(_transform[1]),length(_transform[2]));

    //apply it to the given vectors
    _transform[0] = vec4(normalize(xAxis),0) * scale.x;
    _transform[1] = vec4(normalize(yAxis),0) * scale.y;
    _transform[2] = vec4(normalize(zAxis),0) * scale.z;

    MakeTransformDirty();
}

//this will use the current "up" as reference
void GameObject::setLocalRotation(glm::vec3 forward)
{
    glm::vec3 currentUp = glm::vec3(_transform[2]);
    glm::vec3 right = glm::cross (currentUp, forward);
    glm::vec3 up = glm::cross (forward, right);

    setLocalRotation(right,up,forward);
}

void GameObject::setWorldRotation(glm::vec3 xAxis, glm::vec3 yAxis,glm::vec3 zAxis)
{
    using namespace glm;
    //extract scaling from the current matrix
    vec3 scale = vec3(length(_worldTransform[0]),length(_worldTransform[1]),length(_worldTransform[2]));

    //apply it to the given vectors
    _worldTransform[0] = vec4(normalize(xAxis),0) * scale.x;
    _worldTransform[1] = vec4(normalize(yAxis),0) * scale.y;
    _worldTransform[2] = vec4(normalize(zAxis),0) * scale.z;

    _recalculateLocalTransform();
    MakeChildrenTransformsDirty();
}

//this will use the global up as a reference
void GameObject::setWorldRotation(glm::vec3 forward)
{
    glm::vec3 right = glm::cross (glm::vec3(0,1,0), forward);
    glm::vec3 up = glm::cross (forward, right);

    setLocalRotation(right,up,forward);
}



////////////

glm::vec3 GameObject::getWorldPosition()
{
	return glm::vec3(getWorldTransform()[3]);
}

void GameObject::setWorldPosition(glm::vec3 pos)
{
    _worldTransform[3] = glm::vec4(pos,1);
    _recalculateLocalTransform();
    MakeChildrenTransformsDirty();
}


glm::mat4& GameObject::getWorldTransform()
{
    if(_worldTransformIsDirty)
    {
        _worldTransformIsDirty = false;

        if(_parent!=NULL)
            _worldTransform = _parent->getWorldTransform() * _transform;
        else
            _worldTransform = _transform;
    }
    return _worldTransform;
}

void GameObject::setWorldTransform(const glm::mat4& pTransform)
{
    _worldTransform = pTransform;

    _recalculateLocalTransform();

    MakeTransformDirty();
}

//this is used when the global transform was directly set and the local one needs to be adjusted
void GameObject::_recalculateLocalTransform()
{
    if(_parent!=NULL)
        _transform = glm::inverse(_parent->getWorldTransform()) * _worldTransform;
    else
        _transform = _worldTransform;
}


void GameObject::MakeTransformDirty()
{
    //mark this world transform dirty and the children ones as well, only if it has not already been marked this frame
    if(!_worldTransformIsDirty)
    {
        MakeChildrenTransformsDirty();
        _worldTransformIsDirty = true;
    }

}

void GameObject::MakeChildrenTransformsDirty()
{
    DualLinkNode2<ChildList>* cn = _children.startNode;
    while(cn!=NULL)
    {
        ((GameObject*)cn)->MakeTransformDirty();
        cn = cn->nextNode;
    }
}
////////////

void GameObject::translate(glm::vec3 pTranslation)
{
	setTransform(glm::translate(_transform, pTranslation));
    MakeTransformDirty();
}

void GameObject::scale(glm::vec3 pScale)
{
	setTransform(glm::scale(_transform, pScale));
	MakeTransformDirty();
}

void GameObject::rotate(float pAngle, glm::vec3 pAxis)
{
	setTransform(glm::rotate(_transform, pAngle, pAxis));
	MakeTransformDirty();
}

void GameObject::Update()
{

}

void GameObject::InternalUpdate()
{
    Update();

	DualLinkNode<Component>* cn = _components.startNode;
    while(cn!=NULL)
    {
        Component* cast = ((Component*)cn);
        if(cast->IsActive())
            cast->Update();

        cn = cn->nextNode;
    }

    //recursive internal update of children
    DualLinkNode2<ChildList>* cn2 = _children.startNode;
    while(cn2!=NULL)
    {
        GameObject* cast = ((GameObject*)cn2);
        if(cast->IsActive())
            cast->InternalUpdate();

        cn2 = cn2->nextNode;
    }
}

int GameObject::GetChildCount() {
    return _children.GetCount();
}

GameObject* GameObject::GetChildAt(int pIndex)
{
    return (GameObject*)(_children.GetAt(pIndex));
}

DualLinkList2<ChildList>& GameObject::GetChildren()
{
    return _children;
}

int GameObject::GetComponentsCount() {
    return _components.GetCount();
}

Component* GameObject::GetComponentAt(int pIndex) {
    return (Component*)(_components.GetAt(pIndex));
}

Component* GameObject::FindComponent(std::string name)
{
    DualLinkNode<Component>* cn = _components.startNode;
    while(cn!=NULL)
    {
        Component* comp = (Component*)cn;
        if(comp->getName()==name)
            return comp;
        cn = cn->nextNode;
    }
    return NULL;
}


