#include <cassert>
#include <iostream>
using namespace std;

#include <glm.hpp>

#include "mge/core/GameObject.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/World.hpp"
#include "mge/behaviours/Component.hpp"
#include "mge/core/Renderer.hpp"
#include "mge/collision/Collider.hpp"
#include "glm.hpp"

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

    if(isTranparent)
        Renderer::transparentList.Remove((DualLinkNode<TransparencyList>*)this);
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
    glm::vec3 currentUp = glm::vec3(glm::vec3(0,1,0));
    glm::vec3 right = -glm::cross (currentUp, forward);
    glm::vec3 up = -glm::cross (forward, right);

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
    glm::vec3 right = -glm::cross (glm::vec3(0,1,0), forward);
    glm::vec3 up = -glm::cross (forward, right);

    setWorldRotation(right,up,forward);
}

glm::mat3 GameObject::getWorldRotation()
{
    glm::mat3 ret;
    glm::mat4 tr = getWorldTransform();

    ret[0] = glm::normalize(glm::vec3(tr[0]));
    ret[1] = glm::normalize(glm::vec3(tr[1]));
    ret[2] = glm::normalize(glm::vec3(tr[2]));

    return ret;
}

glm::vec3 GameObject::getLocalScale()
{
    using namespace glm;
	return vec3(length(_transform[0]),length(_transform[1]),length(_transform[2]));
}

void GameObject::setLocalScale(glm::vec3 scale)
{
    using namespace glm;

    //apply it to the given vectors
    _transform[0] = normalize(_transform[0]) * scale.x;
    _transform[1] = normalize(_transform[1]) * scale.y;
    _transform[2] = normalize(_transform[2]) * scale.z;

    MakeTransformDirty();
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
        {
            cast->Update();
            //std::cout<<cast->getName()<<"\n";
        }


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

DualLinkList<Component>& GameObject::GetComponents()
{
    return _components;
}

void GameObject::SetTransparent(bool val)
{
    if(val)
    {
        Renderer::transparentList.Add((DualLinkNode<TransparencyList>*)this);
    }
    else
    {
        if(isTranparent)
            Renderer::transparentList.Remove((DualLinkNode<TransparencyList>*)this);
    }
    isTranparent = val;
}

bool GameObject::IsTransparent()
{
    return isTranparent;
}

bool GameObject::IsActiveInWorld()
{
    GameObject* obj = this;

    while(obj!=NULL)
    {
        if(!(obj->IsActive()))
            return false;

        obj = obj->_parent;
    }

    return true;
}

void GameObject::rotateTo(glm::vec3 target, float speed)
{
    glm::mat3 objRot = getWorldRotation();
    objRot[0]*=-1.0f;//convert matrix from right handed to left handed
    glm::quat objQt = glm::toQuat(objRot);
    glm::vec3 difVec = glm::normalize(target - getWorldPosition());
    glm::vec3 right = glm::cross (glm::vec3(0,1,0), difVec);
    glm::vec3 up = glm::cross (difVec, right);
    glm::mat3 targetMat = glm::mat3(right, up, difVec);
    glm::quat targetQt = glm::toQuat(targetMat);
    objQt = glm::slerp(objQt, targetQt,speed);


    glm::mat3 newMat = glm::mat3_cast(objQt);

    setWorldRotation(-newMat[0], newMat[1], newMat[2]);//convert the left handed matrix back to right handed
    //setLocalRotation(difVec);
    //setLocalRotation(-newMat[0], newMat[1], newMat[2]);
}

BoundingSphere& GameObject::GetRenderBound()
{
    return _renderBounds;
}

void GameObject::RecalculateRenderBound()
{
    using namespace glm;

    if(_mesh == NULL)
        return;

    float meshRadius = _mesh->GetBoundRadius();
    mat4 objMat = this->getWorldTransform();

    _renderBounds.position = this->getWorldPosition();

    //must test for non uniform scaling
    float maxScale = glm::length(objMat[0]);
    float newScale = glm::length(objMat[1]);
    if(newScale > maxScale) maxScale = newScale;
    newScale = glm::length(objMat[2]);
    if(newScale > maxScale) maxScale = newScale;

    _renderBounds.radius = maxScale * meshRadius;
}


