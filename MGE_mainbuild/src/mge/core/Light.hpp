#ifndef LIGHT_H
#define LIGHT_H

#include <mge/core/GameObject.hpp>
#include <glm.hpp>
#include "mge/util/list/DualLinkList.hpp"
#include "mge/util/list/DualLinkNode.hpp"


#define MGE_MAX_LIGHTS 8

class Light : public GameObject, public DualLinkNode<Light>
{
public:
    Light();

    #define MGE_LIGHT_DIRECTIONAL 0
    #define MGE_LIGHT_POINT 1
    #define MGE_LIGHT_SPOTLIGHT 2
    //#dfine MGE_LIGHT_DIRECTIONAL 2;

    int type = MGE_LIGHT_DIRECTIONAL;
    glm::vec3 direction = glm::vec3(0);
    glm::vec3 color = glm::vec3(1);
    //glm::vec3 position = glm::vec3(0);
    glm::vec3 attenuation = glm::vec3(1,0,0);
    float angle = 0.36f;//45 deg, angle for spotlights

    static DualLinkList<Light> const GetLightList();

protected:
    virtual ~Light();

private:
    static DualLinkList<Light> _lightList;
};

#endif // LIGHT_H
