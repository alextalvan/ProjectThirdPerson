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
    glm::vec3 eye = getWorldPosition();
    glm::vec3 center = glm::normalize(pDir) + eye;
    glm::vec3 up = glm::vec3(0,1,0);
    setTransform(glm::lookAt(eye, center, up));
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
    return glm::vec3(getTransform()[2]);
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
    float _angle;
}

Light::~Light()
{
    _lightList.Remove(this);
}

DualLinkList<Light> const  Light::GetLightList()
{
    return _lightList;
}
