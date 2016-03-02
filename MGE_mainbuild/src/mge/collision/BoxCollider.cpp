#include<mge/collision/BoxCollider.hpp>
#include<mge/collision/SphereCollider.hpp>
#include<mge/core/GameObject.hpp>
#include<glm.hpp>
#include<limits>
using namespace std;

BoxCollider::BoxCollider()
{
    _name = "Box Collider";
}

BoxCollider::~BoxCollider()
{

}

void BoxCollider::RefreshBoundingSphere()
{

    if(_owner==NULL)
        return;

    glm::mat4 objMat = _owner->getWorldTransform();
    _bound.position = glm::vec3(objMat[3]);

    float xRad = glm::length(objMat[0] * xSize * 0.5f);
    float yRad = glm::length(objMat[1] * ySize * 0.5f);
    float zRad = glm::length(objMat[2] * zSize * 0.5f);

    _bound.radius = max(xRad,max(yRad,zRad));

}

bool BoxCollider::HitTest(BoxCollider* other)
{

    bool sphereResult = this->BoundingSphereCheck(other);
    if(!sphereResult)
        return false;


    glm::mat4 myMat = _owner->getWorldTransform();
    glm::mat4 otherMat = other->getOwner()->getWorldTransform();

    glm::vec4 myVerts[8];
    glm::vec4 otherVerts[8];

    glm::vec3 MTVaxis;
    float MTVlowestMagnitude = FLT_MAX;

    myVerts[0] = myMat * glm::vec4(xSize*0.5f,ySize*0.5f,zSize * 0.5f, 1);//
    myVerts[1] = myMat * glm::vec4(xSize*0.5f,ySize*0.5f,-zSize * 0.5f, 1);//
    myVerts[2] = myMat * glm::vec4(xSize*0.5f,-ySize*0.5f,zSize * 0.5f, 1);
    myVerts[3] = myMat * glm::vec4(xSize*0.5f,-ySize*0.5f,-zSize * 0.5f, 1);
    myVerts[4] = myMat * glm::vec4(-xSize*0.5f,ySize*0.5f,zSize * 0.5f, 1);
    myVerts[5] = myMat * glm::vec4(-xSize*0.5f,ySize*0.5f,-zSize * 0.5f, 1);
    myVerts[6] = myMat * glm::vec4(-xSize*0.5f,-ySize*0.5f,zSize * 0.5f, 1);
    myVerts[7] = myMat * glm::vec4(-xSize*0.5f,-ySize*0.5f,-zSize * 0.5f, 1);

    otherVerts[0] = otherMat * glm::vec4(other->xSize*0.5f,other->ySize*0.5f,other->zSize * 0.5f, 1);//
    otherVerts[1] = otherMat * glm::vec4(other->xSize*0.5f,other->ySize*0.5f,other->zSize * -0.5f, 1);//
    otherVerts[2] = otherMat * glm::vec4(other->xSize*0.5f,other->ySize*-0.5f,other->zSize * 0.5f, 1);
    otherVerts[3] = otherMat * glm::vec4(other->xSize*0.5f,other->ySize*-0.5f,other->zSize * -0.5f, 1);
    otherVerts[4] = otherMat * glm::vec4(other->xSize*-0.5f,other->ySize*0.5f,other->zSize * 0.5f, 1);
    otherVerts[5] = otherMat * glm::vec4(other->xSize*-0.5f,other->ySize*0.5f,other->zSize * -0.5f, 1);
    otherVerts[6] = otherMat * glm::vec4(other->xSize*-0.5f,other->ySize*-0.5f,other->zSize * 0.5f, 1);
    otherVerts[7] = otherMat * glm::vec4(other->xSize*-0.5f,other->ySize*-0.5f,other->zSize * -0.5f, 1);

    //const float* data = glm::value_ptr(myMat);

    //start with this box's projection directions
    for(int i = 0; i<3;++i)
    {
        glm::vec4 workNormal;
        //workNormal = glm::vec4(0,0,1,0);
        if(i==0) workNormal = glm::normalize(myMat * glm::vec4(1,0,0,0));
        if(i==1) workNormal = glm::normalize(myMat * glm::vec4(0,1,0,0));
        if(i==2) workNormal = glm::normalize(myMat * glm::vec4(0,0,1,0));


        float myMin,otherMin;
        float myMax,otherMax;
        myMin = myMax = glm::dot(myVerts[0],workNormal);
        otherMin = otherMax = glm::dot(otherVerts[0],workNormal);

        for(int j = 1; j < 8 ;++j)
        {
            float myProj = glm::dot(myVerts[j],workNormal);
            float otherProj = glm::dot(otherVerts[j],workNormal);

            if(myProj < myMin) myMin = myProj;
            if(myProj > myMax) myMax = myProj;

            if(otherProj < otherMin) otherMin = otherProj;
            if(otherProj > otherMax) otherMax = otherProj;
        }

        float overallMin = (myMin < otherMin) ? myMin : otherMin;
        float overallMax = (myMax > otherMax) ? myMax : otherMax;

        //test if shadows have gap between them
        float gap = (overallMax - overallMin) - ((myMax - myMin) + (otherMax - otherMin));

        if(gap>0.0f)
            return false;
        //else
        if(abs(gap) < MTVlowestMagnitude)
        {
            MTVaxis = glm::vec3(workNormal);
            MTVlowestMagnitude = abs(gap);
        }
    }

    //const float* data2 = glm::value_ptr(otherMat);

    //continue with other box's projection directions
    for(int i = 0; i<3;++i)
    {
        glm::vec4 workNormal;
        //workNormal = glm::vec4(0,0,1,0);
        if(i==0) workNormal = glm::normalize(otherMat * glm::vec4(1,0,0,0));
        if(i==1) workNormal = glm::normalize(otherMat * glm::vec4(0,1,0,0));
        if(i==2) workNormal = glm::normalize(otherMat * glm::vec4(0,0,1,0));


        float myMin,otherMin;
        float myMax,otherMax;
        myMin = myMax = glm::dot(myVerts[0],workNormal);
        otherMin = otherMax = glm::dot(otherVerts[0],workNormal);

        for(int j = 1; j < 8 ;++j)
        {
            float myProj = glm::dot(myVerts[j],workNormal);
            float otherProj = glm::dot(otherVerts[j],workNormal);

            if(myProj < myMin) myMin = myProj;
            if(myProj > myMax) myMax = myProj;

            if(otherProj < otherMin) otherMin = otherProj;
            if(otherProj > otherMax) otherMax = otherProj;
        }

        float overallMin = (myMin < otherMin) ? myMin : otherMin;
        float overallMax = (myMax > otherMax) ? myMax : otherMax;

        //test if shadows have gap between them
        float gap = (overallMax - overallMin) - ((myMax - myMin) + (otherMax - otherMin));

        if(gap>0.0f)
            return false;
        //else
        if(abs(gap) < MTVlowestMagnitude)
        {
            MTVaxis = glm::vec3(workNormal);
            MTVlowestMagnitude = abs(gap);
        }

    }

    Collider::storedMTV.axis = MTVaxis;
    Collider::storedMTV.magnitude = MTVlowestMagnitude;

    return true;

}

bool BoxCollider::HitTest(SphereCollider* other)
{
    return InternalHitTest(other);
}

bool BoxCollider::InternalHitTest(SphereCollider* other)
{
    glm::mat4 myMat = _owner->getWorldTransform();
    glm::vec3 myVerts[8];

    glm::vec3 MTVaxis;
    float MTVlowestMagnitude = FLT_MAX;

    myVerts[0] = glm::vec3(myMat * glm::vec4(xSize*0.5f,ySize*0.5f,zSize * 0.5f, 1));//
    myVerts[1] = glm::vec3(myMat * glm::vec4(xSize*0.5f,ySize*0.5f,-zSize * 0.5f, 1));//
    myVerts[2] = glm::vec3(myMat * glm::vec4(xSize*0.5f,-ySize*0.5f,zSize * 0.5f, 1));
    myVerts[3] = glm::vec3(myMat * glm::vec4(xSize*0.5f,-ySize*0.5f,-zSize * 0.5f, 1));
    myVerts[4] = glm::vec3(myMat * glm::vec4(-xSize*0.5f,ySize*0.5f,zSize * 0.5f, 1));
    myVerts[5] = glm::vec3(myMat * glm::vec4(-xSize*0.5f,ySize*0.5f,-zSize * 0.5f, 1));
    myVerts[6] = glm::vec3(myMat * glm::vec4(-xSize*0.5f,-ySize*0.5f,zSize * 0.5f, 1));
    myVerts[7] = glm::vec3(myMat * glm::vec4(-xSize*0.5f,-ySize*0.5f,-zSize * 0.5f, 1));

    glm::vec3 sphereCenter = other->getOwner()->getWorldPosition();
    float sphereRadius = other->radius * glm::length((other->getOwner()->getTransform() * glm::vec4(1,0,0,0)));

    //from sphere center to box center
    glm::vec3 workNormal = glm::normalize(_owner->getWorldPosition() - sphereCenter);

    float myMin,otherMin;
    float myMax,otherMax;
    myMin = myMax = glm::dot(myVerts[0],workNormal);
    otherMin = glm::dot(sphereCenter,workNormal);
    otherMax = otherMin + sphereRadius;
    otherMin -= sphereRadius;

    for(int j = 1; j < 8 ;++j)
    {
        float myProj = glm::dot(myVerts[j],workNormal);

        if(myProj < myMin) myMin = myProj;
        if(myProj > myMax) myMax = myProj;
    }

    float overallMin = (myMin < otherMin) ? myMin : otherMin;
    float overallMax = (myMax > otherMax) ? myMax : otherMax;

    //test if shadows have gap between them
    float gap = (overallMax - overallMin) - ((myMax - myMin) + (otherMax - otherMin));

    if(gap>0.0f)
        return false;
    //else
    if(abs(gap) < MTVlowestMagnitude)
    {
        MTVaxis = glm::vec3(workNormal);
        MTVlowestMagnitude = abs(gap);
    }



    //from sphere center to closest vertex

    glm::vec3 closestBoxVertex = myVerts[0];
    float bestDist = glm::distance(myVerts[0],sphereCenter);

    for(int i=1;i<8;++i)
    {
        float d = glm::distance(myVerts[i],sphereCenter);
        if(d<bestDist)
        {
            bestDist = d;
            closestBoxVertex = myVerts[i];
        }
    }

    workNormal = glm::normalize(closestBoxVertex - sphereCenter);

    myMin = myMax = glm::dot(myVerts[0],workNormal);
    otherMin = glm::dot(sphereCenter,workNormal);
    otherMax = otherMin + sphereRadius;
    otherMin -= sphereRadius;

    for(int j = 1; j < 8 ;++j)
    {
        float myProj = glm::dot(myVerts[j],workNormal);

        if(myProj < myMin) myMin = myProj;
        if(myProj > myMax) myMax = myProj;
    }

    overallMin = (myMin < otherMin) ? myMin : otherMin;
    overallMax = (myMax > otherMax) ? myMax : otherMax;

    //test if shadows have gap between them
    gap = (overallMax - overallMin) - ((myMax - myMin) + (otherMax - otherMin));

    if(gap>0.0f)
        return false;
    //else
    if(abs(gap) < MTVlowestMagnitude)
    {
        MTVaxis = glm::vec3(workNormal);
        MTVlowestMagnitude = abs(gap);
    }


    //now to test the box's axii against the sphere
    for(int i = 0; i<3;++i)
    {
        glm::vec3 workNormal;
        //workNormal = glm::vec4(0,0,1,0);
        if(i==0) workNormal = glm::normalize(glm::vec3(myMat * glm::vec4(1,0,0,0)));
        if(i==1) workNormal = glm::normalize(glm::vec3(myMat * glm::vec4(0,1,0,0)));
        if(i==2) workNormal = glm::normalize(glm::vec3(myMat * glm::vec4(0,0,1,0)));


        float myMin,otherMin;
        float myMax,otherMax;
        myMin = myMax = glm::dot(myVerts[0],workNormal);
        otherMin = glm::dot(sphereCenter,workNormal);
        otherMax = otherMin + sphereRadius;
        otherMin -= sphereRadius;

        for(int j = 1; j < 8 ;++j)
        {
            float myProj = glm::dot(myVerts[j],workNormal);

            if(myProj < myMin) myMin = myProj;
            if(myProj > myMax) myMax = myProj;

        }

        float overallMin = (myMin < otherMin) ? myMin : otherMin;
        float overallMax = (myMax > otherMax) ? myMax : otherMax;

        //test if shadows have gap between them
        gap = (overallMax - overallMin) - ((myMax - myMin) + (otherMax - otherMin));

        if(gap>0.0f)
            return false;
        //else
        if(abs(gap) < MTVlowestMagnitude)
        {
            MTVaxis = glm::vec3(workNormal);
            MTVlowestMagnitude = abs(gap);
        }
    }


    Collider::storedMTV.axis = MTVaxis;
    Collider::storedMTV.magnitude = MTVlowestMagnitude;

    return true;
}

bool BoxCollider::HitTest(Collider* other)
{
    if(_owner == NULL || other->getOwner()==NULL)
        return false;

    return other->HitTest(this);
}


bool BoxCollider::RayTest(const Ray& ray, float& distance)
{
    return false;//unimplemented
}


