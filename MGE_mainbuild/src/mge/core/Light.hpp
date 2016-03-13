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
    #define MGE_LIGHT_DIRECTIONAL 0
    #define MGE_LIGHT_POINT 1
    #define MGE_LIGHT_SPOTLIGHT 2
    Light(int pType = MGE_LIGHT_DIRECTIONAL, glm::vec3 pPos = glm::vec3(0), glm::vec3 pDir = glm::vec3(1), glm::vec3 pColor = glm::vec3(1), glm::vec3 pAtt = glm::vec3(0.1f), float pAngle = 0.36f, GameObject* target = NULL);

    static DualLinkList<Light> const GetLightList();
    void setType(int pType);
    void setDirection(glm::vec3 pDir);
    void setColor(glm::vec3 pColor);
    void setAttenuation(glm::vec3 pAtt);
    void setAngle(float pAngle);
    void setFalloff(float falloff);
    int getType();
    glm::vec3 getDirection();
    glm::vec3 getColor();
    glm::vec3 getAttenuation();
    float getAngle();
    float getFalloff();

    static const glm::mat4& GetDirectionalViewMatrix();
    static Light* GetDirectionalLight();
    static void RecalculateDirLightViewMatrix();

protected:
    virtual ~Light();
    void Update() override;
private:
    static DualLinkList<Light> _lightList;
    int _type;
    glm::vec3 _color;
    glm::vec3 _attenuation;
    float _angle;//45 deg, angle for spotlights
    glm::vec3 _direction;
    float _falloffDistance = 10.0f;

    static Light* _dirLight;
    static glm::mat4 _directionalViewMatrix;
    glm::vec3 _storedOffset;
    GameObject* _target;//for directional light "hack" only
};

#endif // LIGHT_H
