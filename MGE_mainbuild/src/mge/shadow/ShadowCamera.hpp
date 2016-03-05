#pragma once

#include <glm.hpp>
#include <mge/core/Camera.hpp>

class ShadowCamera : public GameObject
{
public:
    ShadowCamera();
    void RecalculateFrustum(Camera* mainCam, glm::vec3 lightDir);
    const glm::mat4& getProjection();
protected:
    virtual ~ShadowCamera();
private:
    glm::mat4 _projection;
};
