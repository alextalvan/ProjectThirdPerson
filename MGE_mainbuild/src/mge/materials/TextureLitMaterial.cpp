#include <glm.hpp>

#include "mge/materials/TextureLitMaterial.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/config.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/Renderer.hpp"
#include "glm.hpp"
#include <string>

ShaderProgram* TextureLitMaterial::_shader = NULL;

GLint TextureLitMaterial::_projMatLoc = 0;
GLint TextureLitMaterial::_viewMatLoc = 0;
GLint TextureLitMaterial::_modelMatLoc = 0;
GLint TextureLitMaterial::_lightMatLocNear = 0;
GLint TextureLitMaterial::_lightMatLocFar = 0;
GLint TextureLitMaterial::_lightMatLocMid = 0;

GLint TextureLitMaterial::_diffMapLoc = 0;
GLint TextureLitMaterial::_normalMapLoc = 0;
GLint TextureLitMaterial::_specMapLoc = 0;
GLint TextureLitMaterial::_diffuseTilingLoc = 0;
GLint TextureLitMaterial::_normalTilingLoc = 0;
GLint TextureLitMaterial::_specularTilingLoc = 0;
GLint TextureLitMaterial::_depthMapLocNear = 0;
GLint TextureLitMaterial::_depthMapLocFar = 0;
GLint TextureLitMaterial::_depthMapLocMid = 0;

GLint TextureLitMaterial::_viewPosLoc = 0;

GLint TextureLitMaterial::_matSmoothLoc = 0;
GLint TextureLitMaterial::_matShineLoc = 0;
GLint TextureLitMaterial::_matAmbLoc = 0;

GLboolean TextureLitMaterial::_hasSpecMapLoc = 0;
GLboolean TextureLitMaterial::_hasNormMapLoc = 0;

GLuint TextureLitMaterial::_colorLoc = 0;

TextureLitMaterial::TextureLitMaterial(Texture * pDiffuseTexture, float pSmoothness, float pShininess, float pAmbient,
                                       Texture * pNormalMapTexture, Texture * pSpecularMapTexture,glm::vec4 pColor)
{
    _diffuseTexture=pDiffuseTexture;
    _ambient=pAmbient;
    _smoothness=pSmoothness;
    _shininess=pShininess;
    _normalMapTexture=pNormalMapTexture;
    _specularMapTexture=pSpecularMapTexture;

    color = pColor;

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
        _lightMatLocNear = _shader->getUniformLocation("lightMatrixNear");
        _lightMatLocFar = _shader->getUniformLocation("lightMatrixFar");
        _lightMatLocMid = _shader->getUniformLocation("lightMatrixMid");

        _diffMapLoc = _shader->getUniformLocation("material.diffuseMap");
        _normalMapLoc = _shader->getUniformLocation("material.normalMap");
        _specMapLoc = _shader->getUniformLocation("material.specularMap");
        _diffuseTilingLoc = _shader->getUniformLocation("material.tilingDiffuse");
        _normalTilingLoc = _shader->getUniformLocation("material.tilingNormal");
        _specularTilingLoc = _shader->getUniformLocation("material.tilingSpecular");
        _depthMapLocNear = _shader->getUniformLocation("depthMapNear");
        _depthMapLocFar = _shader->getUniformLocation("depthMapFar");
        _depthMapLocMid = _shader->getUniformLocation("depthMapMid");

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

void TextureLitMaterial::setNormalMapTexture (Texture* pNormalMapTexture) {
    _normalMapTexture = pNormalMapTexture;
    normalMap = true;
}

void TextureLitMaterial::setSpecularMapTexture (Texture* pSpecularMapTexture) {
    _specularMapTexture = pSpecularMapTexture;
    specularMap = true;
}

void TextureLitMaterial::SetDiffuseTiling(float val)
{
    _diffuseTiling = val;
}

void TextureLitMaterial::SetNormalTiling(float val)
{
    _normalTiling = val;
}

void TextureLitMaterial::SetSpecularTiling(float val)
{
    _specularTiling = val;
}

void TextureLitMaterial::render(GameObject* pGameObject, Camera* pCamera) {
    if (!_diffuseTexture) return;

    _shader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _diffuseTexture->getId());
    glUniform1i (_diffMapLoc, 0);
    glUniform1f(_diffuseTilingLoc, _diffuseTiling);

    if (normalMap) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _normalMapTexture->getId());
        glUniform1i (_normalMapLoc, 1);
        glUniform1f(_normalTilingLoc, _normalTiling);
    }

    if (specularMap) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _specularMapTexture->getId());
        glUniform1i (_specMapLoc, 2);
        glUniform1f(_specularTilingLoc, _specularTiling);
    }

    if (Renderer::getShadowDepthMapFar()) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, Renderer::getShadowDepthMapFar());
        glUniform1i (_depthMapLocFar, 3);
    }

    if (Renderer::getShadowDepthMapFar()) {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, Renderer::getShadowDepthMapNear());
        glUniform1i (_depthMapLocNear, 4);
    }

    if (Renderer::getShadowDepthMapMid()) {
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, Renderer::getShadowDepthMapMid());
        glUniform1i (_depthMapLocMid, 5);
    }

	glUniform1f(_matSmoothLoc, _smoothness);
	glUniform1f(_matShineLoc, _shininess);
	glUniform1f(_matAmbLoc, _ambient);
    glUniform1f(_hasNormMapLoc, normalMap);
    glUniform1f(_hasSpecMapLoc, specularMap);

    //pass in light data
    int index = 0;

    DualLinkList<Light> list = Light::GetLightList();



    DualLinkNode<Light>* cn = list.startNode;

    //GLfloat near_plane = 1.0f, far_plane = 25.0f;

    while(cn!=NULL && index < MGE_MAX_LIGHTS)
    {
        Light* light = (Light*)cn;

        if(!light->IsActive())//do not render with inactive lights
        {
            cn = cn->nextNode;
            continue;
        }

        string indexString ="LightArray[" + std::to_string(index) + "]";
        GLuint loc;

        int lightType = light->getType();


        if (lightType == MGE_LIGHT_DIRECTIONAL)
        {
            loc = _shader->getUniformLocation(indexString + ".type");
            glUniform1i(loc,lightType);

            loc = _shader->getUniformLocation(indexString + ".color");
            glUniform3fv(loc,1,glm::value_ptr(light->getColor()));

            loc = _shader->getUniformLocation(indexString + ".direction");
            glUniform3fv(loc,1,glm::value_ptr(light->getDirection()));
            //loc = _shader->getUniformLocation(indexString + ".position");
            //glUniform3fv(loc,1,glm::value_ptr(light->getLocalPosition()));

            glm::mat4 lightProjection, lightView;
            lightView = Light::GetDirectionalViewMatrix();
            glm::mat4 lightSpaceMatrix;


            lightProjection = Renderer::GetFarShadowOrtho();
            lightSpaceMatrix = lightProjection * lightView;
            glUniformMatrix4fv(_lightMatLocFar, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

            lightProjection = Renderer::GetNearShadowOrtho();
            lightSpaceMatrix = lightProjection * lightView;
            glUniformMatrix4fv(_lightMatLocNear, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

            lightProjection = Renderer::GetMidShadowOrtho();
            lightSpaceMatrix = lightProjection * lightView;
            glUniformMatrix4fv(_lightMatLocMid, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
            ++index;
        }
        else
        if ((!pGameObject->ignoreNonDirLights) && lightType == MGE_LIGHT_POINT)
        {
            glm::vec3 lightPos = light->getWorldPosition();
            float dist = glm::distance(lightPos, pGameObject->getWorldPosition());

            if(dist > light->getFalloff() + pGameObject->GetRenderBound().radius)
            {
                cn = cn->nextNode;
                continue;
            }

            loc = _shader->getUniformLocation(indexString + ".type");
            glUniform1i(loc,lightType);

            loc = _shader->getUniformLocation(indexString + ".color");
            glUniform3fv(loc,1,glm::value_ptr(light->getColor()));

            loc = _shader->getUniformLocation(indexString + ".position");
            glUniform3fv(loc,1,glm::value_ptr(lightPos));

            loc = _shader->getUniformLocation(indexString + ".attenuation");
            glUniform3fv(loc,1,glm::value_ptr(light->getAttenuation()));

            ++index;
        }
        //++index;
        cn = cn->nextNode;
    }

    glUniform1i(_shader->getUniformLocation("lightCount"),index);

	//set view pos
	glUniform3fv(_viewPosLoc, 1, glm::value_ptr(pCamera->getWorldPosition()));

	//color
	glUniform4fv(_colorLoc,1,glm::value_ptr(color));

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
