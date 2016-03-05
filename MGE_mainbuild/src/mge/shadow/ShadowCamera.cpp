#include <mge/shadow/ShadowCamera.hpp>

ShadowCamera::ShadowCamera()
{

}

ShadowCamera::~ShadowCamera()
{

}

void ShadowCamera::RecalculateFrustum(Camera* mainCam, glm::vec3 lightDir)
{
    using namespace glm;
    float far = mainCam->far * 0.5f;//we want shadows to be cast up until half the far distance
    float near = mainCam->near;
    float halfTan = glm::tan(glm::radians(mainCam->fov * 0.5f));
    float aspect = mainCam->aspectRatio;

    vec3 camPos = mainCam->getWorldPosition();
    mat3 camRot = mainCam->getWorldRotation();
    //camRot[2] = camRot[2] * -1.0f;

    vec3 camFrustBounds[8];

    float hNear = 2 * halfTan * near;//height of near plane
    float wNear = hNear * aspect;//width of near plane

    float hFar = 2 * halfTan * far;//height of far plane
    float wFar = hFar * aspect;//width of far plane

    vec3 nearPlanePos = camPos + camRot[2] * near;
    vec3 farPlanePos = camPos + camRot[2] * far;

    //verts of camera near clip plane
    camFrustBounds[0] = nearPlanePos - camRot[0] * (wNear * 0.5f) - camRot[1] * (hNear * 0.5f);
    camFrustBounds[1] = nearPlanePos + camRot[0] * (wNear * 0.5f) - camRot[1] * (hNear * 0.5f);
    camFrustBounds[2] = nearPlanePos - camRot[0] * (wNear * 0.5f) + camRot[1] * (hNear * 0.5f);
    camFrustBounds[3] = nearPlanePos + camRot[0] * (wNear * 0.5f) + camRot[1] * (hNear * 0.5f);

    //verts of far plane
    camFrustBounds[4] = farPlanePos - camRot[0] * (wFar * 0.5f) - camRot[1] * (hFar * 0.5f);
    camFrustBounds[5] = farPlanePos + camRot[0] * (wFar * 0.5f) - camRot[1] * (hFar * 0.5f);
    camFrustBounds[6] = farPlanePos - camRot[0] * (wFar * 0.5f) + camRot[1] * (hFar * 0.5f);
    camFrustBounds[7] = farPlanePos + camRot[0] * (wFar * 0.5f) + camRot[1] * (hFar * 0.5f);

    //now that we have computed all the vertices of the camera frustum, we have to
    //compute the smallest bounding box that has a forward vector equal to the light direction
    //and contains the camera frustum

    float minX,minY,minZ; minX = minY = minZ = FLT_MAX;
    float maxX,maxY,maxZ; maxX = maxY = maxZ = FLT_MAX * -1.0f;


    this->setWorldRotation(lightDir);
    mat3 lightRot = this->getWorldRotation();
//    vec3 lightZaxis = normalize(lightDir);
//    vec3 lightXaxis = normalize(cross(vec3(0,1,0), lightZaxis));
//    vec3 lightYaxis =

    for(int i=0;i<8;++i)
    {
        float xProj = glm::dot(camFrustBounds[i],lightRot[0]);
        float yProj = glm::dot(camFrustBounds[i],lightRot[1]);
        float zProj = glm::dot(camFrustBounds[i],lightRot[2]);

        if(xProj < minX) minX = xProj; if(xProj > maxX) maxX = xProj;
        if(yProj < minY) minY = yProj; if(yProj > maxY) maxY = yProj;
        if(zProj < minZ) minZ = zProj; if(zProj > maxZ) maxZ = zProj;
    }

    //we have the orthographic projection bounds, we still need to compute
    //the shadow camera's position

    vec3 camFrustumCenter = nearPlanePos + camRot[2] * ((far - near) * 0.5f);
    float camFrustumCenterProj = glm::dot(camFrustumCenter,lightRot[2]);
    vec3 shadowCamPos = camFrustumCenter - lightRot[2] * abs(camFrustumCenterProj - minZ);
    //the "magic" value 1 here is to ensure the orthographic projection will have a near plane of 1.0f;


    this->setWorldPosition(shadowCamPos);

    //setWorldTransform(glm::lookAt(shadowCamPos,shadowCamPos + lightDir,vec3(0,1,0)));

    //now computing the orthographic bounds in local space
    float myZProj = dot(lightRot[2], shadowCamPos);
    float orthoNearZ = 1.0f;
    float orthoFarZ = maxZ - myZProj;

    float myXProj = dot(lightRot[0], shadowCamPos);
    float orthoMinX = -abs(myXProj - minX);
    float orthoMaxX = abs(maxX - myXProj);

    float myYProj = dot(lightRot[1], shadowCamPos);
    float orthoMinY = -abs(myYProj - minY);
    float orthoMaxY = abs(maxY - myYProj);

    _projection = glm::ortho(orthoMinX,orthoMaxX,orthoMinY,orthoMaxY,orthoNearZ,orthoFarZ);
    //std::cout<<orthoMinX<<" "<<orthoMaxX<<" "<<orthoMinY<<" "<<orthoMaxY<<" "<<orthoNearZ<<" "<<orthoFarZ<<"\n";

}


const glm::mat4& ShadowCamera::getProjection()
{
    return _projection;
}
