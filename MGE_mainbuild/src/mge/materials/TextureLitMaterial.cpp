#include <glm.hpp>

#include "mge/materials/TextureLitMaterial.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/config.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/Renderer.hpp"
#include <string>

ShaderProgram* TextureLitMaterial::_shader = NULL;

GLint TextureLitMaterial::_projMatLoc = 0;
GLint TextureLitMaterial::_viewMatLoc = 0;
GLint TextureLitMaterial::_modelMatLoc = 0;
GLint TextureLitMaterial::_lightMatLoc = 0;

GLint TextureLitMaterial::_diffMapLoc = 0;
GLint TextureLitMaterial::_normalMapLoc = 0;
GLint TextureLitMaterial::_specMapLoc = 0;
GLint TextureLitMaterial::_depthMapLoc = 0;
GLint TextureLitMaterial::_depthCubeMapLoc = 0;

GLint TextureLitMaterial::_viewPosLoc = 0;

GLint TextureLitMaterial::_matSmoothLoc = 0;
GLint TextureLitMaterial::_matShineLoc = 0;
GLint TextureLitMaterial::_matAmbLoc = 0;

GLboolean TextureLitMaterial::_hasSpecMapLoc = 0;
GLboolean TextureLitMaterial::_hasNormMapLoc = 0;

GLuint TextureLitMaterial::_colorLoc = 0;

TextureLitMaterial::TextureLitMaterial(Texture * pDiffuseTexture, float pSmoothness, float pShininess, float pAmbient, Texture * pSpecularMapTexture, Texture * pNormalMapTexture)
{
    _diffuseTexture = pDiffuseTexture;
    _smoothness=pSmoothness;
    _ambient=pAmbient;
    _normalMapTexture=pNormalMapTexture;
    _shininess=pShininess;
    _specularMapTexture=pSpecularMapTexture;

    _lazyInitializeShader();

    if (!_specularMapTexture)
        specularMap = false;
    else
        specularMap = true;

    if (!_normalMapTexture)
        normalMap = false;
    else
        normalMap = true;
}

TextureLitMaterial::~TextureLitMaterial() {}

void TextureLitMaterial::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH + "texture_fragment_lit.vs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH + "texture_fragment_lit.fs");

        glBindAttribLocation(GL_VERTEX_SHADER, 0, "vertex");
        glBindAttribLocation(GL_VERTEX_SHADER, 1, "normal");
        glBindAttribLocation(GL_VERTEX_SHADER, 2, "uv");
        glBindAttribLocation(GL_VERTEX_SHADER, 3, "tangent");

        glGetError();

        _shader->finalize();

        _projMatLoc = _shader->getUniformLocation("projectionMatrix");
        _viewMatLoc = _shader->getUniformLocation("viewMatrix");
        _modelMatLoc = _shader->getUniformLocation("modelMatrix");
        _lightMatLoc = _shader->getUniformLocation("lightMatrix");

        _diffMapLoc = _shader->getUniformLocation("material.diffuseMap");
        _normalMapLoc = _shader->getUniformLocation("material.normalMap");
        _specMapLoc = _shader->getUniformLocation("material.specularMap");
        _depthMapLoc = _shader->getUniformLocation("depthMap");
        _depthCubeMapLoc = _shader->getUniformLocation("depthCubeMap");

        _viewPosLoc = _shader->getUniformLocation("viewPos");

        _matSmoothLoc = _shader->getUniformLocation("material.smoothness");
        _matShineLoc = _shader->getUniformLocation("material.shininess");
        _matAmbLoc = _shader->getUniformLocation("material.ambient");

        _hasNormMapLoc = _shader->getUniformLocation("hasNormalMap");
        _hasSpecMapLoc = _shader->getUniformLocation("hasSpecMap");

        _colorLoc = _shader->getUniformLocation("color");
    }
}

void TextureLitMaterial::setDiffuseTexture (Texture* pDiffuseTexture) {
    _diffuseTexture = pDiffuseTexture;
}

void TextureLitMaterial::setSpecularMapTexture (Texture* pSpecularMapTexture) {
    _specularMapTexture = pSpecularMapTexture;
    specularMap = true;
}

void TextureLitMaterial::setNormalMapTexture (Texture* pNormalMapTexture) {
    _normalMapTexture = pNormalMapTexture;
    normalMap = true;
}

void TextureLitMaterial::render(World* pWorld, GameObject* pGameObject, Camera* pCamera) {
    if (!_diffuseTexture) return;

    _shader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _diffuseTexture->getId());
    glUniform1i (_diffMapLoc, 0);

    if (specularMap) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _specularMapTexture->getId());
        glUniform1i (_specMapLoc, 1);
    }

    if (normalMap) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _normalMapTexture->getId());
        glUniform1i (_normalMapLoc, 2);
    }

    if (Renderer::getDepthMap()) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, Renderer::getDepthMap());
        glUniform1i (_depthMapLoc, 3);
    }

    if (Renderer::getDepthCubeMap()) {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, Renderer::getDepthCubeMap());
        glUniform1i (_depthCubeMapLoc, 4);
    }

	glUniform1f(_matSmoothLoc, _smoothness);
	glUniform1f(_matShineLoc, _shininess);
	glUniform1f(_matAmbLoc, _ambient);
    glUniform1f(_hasSpecMapLoc, specularMap);
    glUniform1f(_hasNormMapLoc, normalMap);

    //pass in light data
    int index = 0;

    DualLinkList<Light> list = Light::GetLightList();

    glUniform1i(_shader->getUniformLocation("lightCount"),list.GetCount());

    DualLinkNode<Light>* cn = list.startNode;

    GLfloat near_plane = 1.0f, far_plane = 25.0f;

    while(cn!=NULL && index < MGE_MAX_LIGHTS)
    {
        string indexString = std::to_string(index);
        Light* light = (Light*)cn;
        GLuint loc;

        int lightType = light->getType();
        loc = _shader->getUniformLocation("LightArray[" + indexString + "].type");
        glUniform1i(loc,lightType);

        loc = _shader->getUniformLocation("LightArray[" + indexString + "].color");
        glUniform3fv(loc,1,glm::value_ptr(light->getColor()));

        if (lightType == MGE_LIGHT_DIRECTIONAL) {
            loc = _shader->getUniformLocation("LightArray[" + indexString + "].direction");
            glUniform3fv(loc,1,glm::value_ptr(light->getDirection()));
            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            GLfloat near_plane = 1.0f, far_plane = 50.0f;
            lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
            lightView = light->getWorldTransform();
            lightSpaceMatrix = lightProjection * lightView;
            glUniformMatrix4fv(_lightMatLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        } else if (lightType == MGE_LIGHT_POINT) {
            GLfloat far = 25.0f;
            loc = _shader->getUniformLocation("far_plane");
            glUniform1f(loc,far);
            loc = _shader->getUniformLocation("LightArray[" + indexString + "].position");
            glUniform3fv(loc,1,glm::value_ptr(light->getWorldPosition()));
            loc = _shader->getUniformLocation("LightArray[" + indexString + "].attenuation");
            glUniform3fv(loc,1,glm::value_ptr(light->getAttenuation()));
        }
        /*
        else if (lightType == MGE_LIGHT_SPOTLIGHT) {
            loc = _shader->getUniformLocation("LightArray[" + indexString + "].position");
            glUniform3fv(loc,1,glm::value_ptr(light->getWorldPosition()));
            loc = _shader->getUniformLocation("LightArray[" + indexString + "].attenuation");
            glUniform3fv(loc,1,glm::value_ptr(light->getAttenuation()));
            loc = _shader->getUniformLocation("LightArray[" + indexString + "].direction");
            glUniform3fv(loc,1,glm::value_ptr(light->getDirection()));
            loc = _shader->getUniformLocation("LightArray[" + indexString + "].angle");
            glUniform1f(loc,glm::cos(light->getAngle()));
        }
        */
        ++index;
        cn = cn->nextNode;
    }

	//set view pos
	glUniform3fv(_viewPosLoc, 1, glm::value_ptr(pCamera->getWorldPosition()));

	//color
	glUniform3fv(_colorLoc,1,glm::value_ptr(color));

    //pass in all MVP matrices separately
    glUniformMatrix4fv ( _projMatLoc,  1, GL_FALSE, glm::value_ptr(pCamera->getProjection()));
    glUniformMatrix4fv ( _viewMatLoc,  1, GL_FALSE, glm::value_ptr(pCamera->getView()));
    glUniformMatrix4fv ( _modelMatLoc, 1, GL_FALSE, glm::value_ptr(pGameObject->getWorldTransform()));

    //now inform mesh of where to stream its data
    pGameObject->getMesh()->streamToOpenGL(0, 1, 2, 3);

    //glGetError();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
