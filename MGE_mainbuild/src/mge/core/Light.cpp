#include <mge/core/Light.hpp>

std::vector<Light*> Light::_lightList;

Light::Light() : GameObject("light")
{
    _lightList.push_back(this);
}

Light::~Light()
{
    //_lightList.erase(this);
}

std::vector<Light*> const  Light::GetLightList()
{
    return _lightList;
}
