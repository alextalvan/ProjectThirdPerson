#include <mge/core/Light.hpp>

DualLinkList<Light> Light::_lightList;

Light::Light() : GameObject("light")
{
    _lightList.Add(this);
}

Light::~Light()
{
    _lightList.Remove(this);
}

DualLinkList<Light> const  Light::GetLightList()
{
    return _lightList;
}
