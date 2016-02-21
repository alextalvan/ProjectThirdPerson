#include <mge/core/Light.hpp>

DualLinkList<Light> Light::_lightList;

Light::Light(int pType, glm::vec3 pPos, glm::vec3 pDir, glm::vec3 pColor, glm::vec3 pAtt, float pAngle)
: GameObject("light", pPos), _type(pType), _color(pColor), _attenuation(pAtt), _angle(glm::radians(pAngle))
{
    setDirection(pDir);
    _lightList.Add(this);
}

void Light::setType(int pType)
{
    _type = pType;
}

void Light::setDirection(glm::vec3 pDir)
{
    setWorldRotation(glm::normalize(pDir));
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
    return glm::normalize(glm::vec3(getWorldTransform()[2]));
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
