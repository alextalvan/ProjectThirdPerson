#include <mge/core/Light.hpp>
#include <mge/util/Input.hpp>

DualLinkList<Light> Light::_lightList;

Light::Light(int pType, glm::vec3 pPos, glm::vec3 pDir, glm::vec3 pColor, glm::vec3 pAtt, float pAngle, GameObject* target)
: GameObject("light", pPos), _type(pType), _color(pColor), _attenuation(pAtt), _angle(glm::radians(pAngle))
{
    setDirection(pDir);
    if(pType == MGE_LIGHT_DIRECTIONAL)
    {
        _target = target;
        _storedOffset = getLocalPosition();
    }

    _lightList.Add(this);
}

void Light::setType(int pType)
{
    _type = pType;
}

void Light::setDirection(glm::vec3 pDir)
{
    //glm::vec3 forward = glm::normalize(getWorldPosition() - getWorldPosition() + pDir);
    //glm::vec3 right = glm::cross (glm::vec3(0,1,0), forward);
    //glm::vec3 up = glm::cross (forward, right);

    //setWorldTransform( glm::mat4 (glm::vec4(right,0), glm::vec4(up,0), glm::vec4(forward,0), glm::vec4(getWorldPosition(),1) ) );
    setTransform(glm::lookAt(getWorldPosition(), getWorldPosition() + pDir, glm::vec3(0,1,0)));
    _direction = glm::normalize(pDir);
    //setWorldRotation(glm::normalize(pDir));
}

void Light::setColor(glm::vec3 pColor)
{
    _color = pColor;
}

void Light::setAttenuation(glm::vec3 pAtt)
{
    _attenuation = pAtt;
}

void Light::setAngle(float pAngle)
{
    _angle = glm::radians(pAngle);
}

int Light::getType()
{
    return _type;
}

glm::vec3 Light::getDirection()
{
    return _direction;
}

glm::vec3 Light::getColor()
{
    return _color;
}

glm::vec3 Light::getAttenuation()
{
    return _attenuation;
}

float Light::getAngle()
{
    return _angle;
}

Light::~Light()
{
    _lightList.Remove(this);
}

DualLinkList<Light> const  Light::GetLightList()
{
    return _lightList;
}
//test
void Light::Update()
{
    if(_type == MGE_LIGHT_DIRECTIONAL)
    {
        glm::vec3 tPos = _target->getWorldPosition();
        setLocalPosition(tPos + _storedOffset);
    }
}
