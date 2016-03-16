#include <iostream>
using namespace std;

#include <glm.hpp>
#include "mge/core/Camera.hpp"
#include<mge/behaviours/Component.hpp>
#include<mge/core/GameObject.hpp>
//#include<mge/behaviours/FirstPersonLook.hpp>
#include<mge/core/Mesh.hpp>

Camera::Camera( std::string pName, glm::vec3 pPosition, glm::mat4 pProjectionMatrix)
:	GameObject(pName, pPosition)//, _projection(pProjectionMatrix)
{
    _projection = glm::perspective (glm::radians(60.0f), -16.0f/9.0f, 0.1f, 5000.0f  );
    _view = glm::inverse(getWorldTransform());


    fov = 60.0f;
    aspectRatio = 16.0f/9.0f;
    near = 0.1f;
    far = 5000.0f;
}

Camera::~Camera()
{
	//dtor
}

glm::mat4& Camera::getProjection()
{
    return _projection;
}

glm::mat4& Camera::getView()
{
    if(_viewMatrixDirty)
    {
        _viewMatrixDirty = false;
        _view = glm::inverse(getWorldTransform());
    }

    return _view;
}

void Camera::MakeTransformDirty()
{
    _viewMatrixDirty = true;
    GameObject::MakeTransformDirty();
}

void Camera::_recalculateLocalTransform()
{
    _viewMatrixDirty = true;
    GameObject::_recalculateLocalTransform();
}


//frustrum culling
void Camera::RecalculateFrustumCache()
{

    using namespace glm;
    mat4 myMat = getWorldTransform();

    vec3 zaxis = normalize(vec3(myMat[2]));
    vec3 yaxis = normalize(vec3(myMat[1]));
    vec3 xaxis = normalize(vec3(myMat[0]));

    vec3 camPos = getWorldPosition(); //vec3(myMat[3]);
    //_cullingCache.camPos = camPos;

    float aspect = aspectRatio;//aspect ratio
    float halfTan = glm::tan(glm::radians(fov * 0.5f));//tangent of half the fov angle
    float nearPlane = near;
    float farPlane = far;


    float hNear = 2 * halfTan * nearPlane;
    float wNear = hNear * aspect;

    _cullingCache.forwardNorm = zaxis * -1.0f;
    _cullingCache.backNorm = zaxis;

    vec3 nearPos = zaxis * nearPlane;
    _cullingCache.backOrigin = dot(_cullingCache.backNorm,(nearPos + camPos));


    //vec3 farPos = zaxis * farPlane;
    //_cullingCache.forwardOrigin = dot(_cullingCache.forwardNorm,(farPos + camPos));

    vec3 nearLeft = xaxis * (wNear * -0.5f) + nearPos;
    vec3 leftNormal = normalize(glm::cross(yaxis,nearLeft));
    _cullingCache.leftNorm = leftNormal;
    _cullingCache.leftOrigin = dot(leftNormal,camPos);

    vec3 nearRight = xaxis * (wNear * 0.5f) + nearPos;
    vec3 rightNormal = normalize(-glm::cross(yaxis,nearRight));
    _cullingCache.rightNorm = rightNormal;
    _cullingCache.rightOrigin = dot(rightNormal,camPos);

    vec3 nearUp =  yaxis * (hNear * -0.5f) + nearPos;
    vec3 upNormal = normalize(-cross(xaxis,nearUp));
    _cullingCache.upNorm = upNormal;
    _cullingCache.upOrigin = dot(upNormal,camPos);

    vec3 nearDown = yaxis * (hNear * 0.5f) + nearPos;
    vec3 downNormal = normalize(cross(xaxis,nearDown));
    _cullingCache.downNorm = downNormal;
    _cullingCache.downOrigin = dot(downNormal,camPos);
}

bool Camera::FrustumCheck(GameObject* obj)
{
    using namespace glm;

//    float meshRadius = obj->getMesh()->GetBoundRadius();
//    mat4 objMat = obj->getWorldTransform();
//
//    vec3 objPos = obj->getWorldPosition();
//
//    //must test for non uniform scaling
//    float maxScale = glm::length(objMat[0]);
//    float newScale = glm::length(objMat[1]);
//    if(newScale > maxScale) maxScale = newScale;
//    newScale = glm::length(objMat[2]);
//    if(newScale > maxScale) maxScale = newScale;
//
//    float objRadius = maxScale * meshRadius;
    vec3 objPos = obj->GetRenderBound().position;
    float objRadius = obj->GetRenderBound().radius;

    //back culling first
    if(dot(_cullingCache.backNorm,objPos) > _cullingCache.backOrigin + objRadius)
       return false;

    //left and right after
    if(dot(_cullingCache.leftNorm,objPos) > _cullingCache.leftOrigin + objRadius)
        return false;

    if(dot(_cullingCache.rightNorm,objPos) > _cullingCache.rightOrigin + objRadius)
       return false;

    //up and down
    if(dot(_cullingCache.upNorm,objPos) > _cullingCache.upOrigin + objRadius)
       return false;

    if(dot(_cullingCache.downNorm,objPos) > _cullingCache.downOrigin + objRadius)
        return false;

    //for now, skipped forward culling
    //if(dot(_cullingCache.forwardNorm,objPos) < _cullingCache.forwardOrigin - objRadius)
    //   return false;

    return true;

}

//void Camera::Update()
//{
//    if(_parent!=NULL)
//    {
//        glm::vec3 newPos = glm::lerp(getWorldPosition(),_parent->getWorldPosition(),followLerpFactor);
//        setWorldPosition(newPos);
//    }
//}

