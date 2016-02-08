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
    _mesh( NULL ),_components(), _material(NULL), _world(NULL)
{
}

GameObject::~GameObject()
{
    //detach all children
    cout << getName()<<" was released.";
}

void GameObject::Destroy()
{

    Destroyable::Destroy();
    SetActive(false);

    setParent(NULL);

    //destroy all my components
    for (auto i = _components.begin(); i != _components.end(); ++i)
    {
       (*i)->Destroy();
    }

    //destroy children as well
     for (auto i = _children.begin(); i != _children.end(); ++i)
    {
       (*i)->Destroy();
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

void GameObject::AttachComponent(Component* comp)
{
	_components.push_back(comp);
	comp->setOwner(this);
}

void GameObject::DetachComponent(Component* comp)
{
    for (auto i = _components.begin(); i != _components.end(); ++i)
    {
        if (*i == comp)
        {
            (*i)->setOwner(NULL);
            _components.erase(i);
            return;
        }
    }
}

void GameObject::DestroyComponent(Component* comp)
{
    //todo: desroy
    DetachComponent(comp);
    comp->Destroy();
}

void GameObject::setParent (GameObject* pParent) {
    //remove from previous parent
    if (_parent != NULL) {
        _parent->_innerRemove(this);
        _parent = NULL;
        _world = NULL;
    }

    //set new parent
    if (pParent != NULL) {
        _parent = pParent;
        _parent->_innerAdd(this);
        //pass on world to child
        _world = _parent->_world;
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
	_children.push_back(pChild);
}

void GameObject::_innerRemove (GameObject* pChild) {
    for (auto i = _children.begin(); i != _children.end(); ++i) {
        if (*i == pChild) {
            (*i)->_parent = NULL;
            _children.erase(i);
            return;
        }
    }
}

void GameObject::AddChild (GameObject* pChild) {
    pChild->setParent(this);
}

void GameObject::RemoveChild (GameObject* pChild) {
    pChild->setParent(NULL);
}

////////////

glm::vec3 GameObject::getWorldPosition()
{
	return glm::vec3(getWorldTransform()[3]);
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

    if(_parent!=NULL)
        _transform = glm::inverse(_parent->getWorldTransform()) * _worldTransform;
    else
        _transform = _worldTransform;

    MakeTransformDirty();
}


void GameObject::MakeTransformDirty()
{
    //mark this world transform dirty and the children ones as well, only if it has not already been marked this frame
    if(!_worldTransformIsDirty)
    {
        for (auto i = _children.begin(); i != _children.end(); ++i)
        {
            (*i)->MakeTransformDirty();
        }

        _worldTransformIsDirty = true;
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

	for (int i = _components.size()-1; i >= 0; --i )
    {
        if(_components[i]->IsActive())
            _components[i]->Update();
    }

    //recursive internal update of children
    for (int i = _children.size()-1; i >= 0; --i )
    {
        if(_children[i]->IsActive())
            _children[i]->InternalUpdate();
    }
}

int GameObject::GetChildCount() {
    return _children.size();
}

GameObject* GameObject::GetChildAt(int pIndex) {
    return _children[pIndex];
}

int GameObject::GetComponentsCount() {
    return _components.size();
}

Component* GameObject::GetComponentAt(int pIndex) {
    return _components[pIndex];
}


