#include <glm.hpp>

#include "mge/materials/TextureLitMaterial.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/config.hpp"
#include "mge/core/Light.hpp"
#include <string>

ShaderProgram* TextureLitMaterial::_depthShader = NULL;
ShaderProgram* TextureLitMaterial::_shader = NULL;

TextureLitMaterial::TextureLitMaterial(Texture * pDiffuseTexture):_diffuseTexture(pDiffuseTexture) {
    _lazyInitializeShader();
}

TextureLitMaterial::~TextureLitMaterial() {}

void TextureLitMaterial::_lazyInitializeShader() {
    if (!_depthShader) {
        _depthShader = new ShaderProgram();
        _depthShader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH+"depth_shader.vs");
        _depthShader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH+"depth_shader.fs");
        _depthShader->finalize();
    }
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH+"texture_fragment_lit.vs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH+"texture_fragment_lit.fs");
        _shader->finalize();
    }
}

void TextureLitMaterial::setDiffuseTexture (Texture* pDiffuseTexture) {
    _diffuseTexture = pDiffuseTexture;
}

void TextureLitMaterial::render(World* pWorld, GameObject* pGameObject, Camera* pCamera) {
    if (!_diffuseTexture) return;

    _depthShader->use();
    _shader->use();

    //setup texture slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _diffuseTexture->getId());
    glUniform1i (_shader->getUniformLocation("textureDiffuse"), 0);

    //pass in light data
    int index = 0;

    DualLinkList<Light> list = Light::GetLightList();

    glUniform1i(_shader->getUniformLocation("lightCount"),list.GetCount());

    DualLinkNode<Light>* cn = list.startNode;

    GLfloat near_plane = 1.0f, far_plane = 7.5f;

    while(cn!=NULL && index < MGE_MAX_LIGHTS)
    {
        string indexString = std::to_string(index);
        Light* light = (Light*)cn;
        GLuint loc;

        loc = _shader->getUniformLocation("LightArray["+indexString+"].type");
        glUniform1i(loc,light->type);

        loc = _shader->getUniformLocation("LightArray["+indexString+"].position");
        glm::vec3 lightPos = light->getWorldPosition();
        glUniform3fv(loc,1,glm::value_ptr(lightPos));

        loc = _shader->getUniformLocation("LightArray["+indexString+"].direction");

        if(light->type == MGE_LIGHT_SPOTLIGHT)
        {
            glm::vec3 lightForward = glm::vec3(light->getWorldTransform()[2]);
            glUniform3fv(loc,1,glm::value_ptr(lightForward));
            //std::cout<<"spotlight dir: "<<lightForward<<"\n";
        }
        else if (light->type == MGE_LIGHT_DIRECTIONAL)
        {
            glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(1.0));
            glm::mat4 lightSpaceMatrix = lightProjection * lightView;
            glUniformMatrix4fv (_depthShader->getUniformLocation("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        }
        else
        {
            glUniform3fv(loc,1,glm::value_ptr(light->direction));
        }



        loc = _shader->getUniformLocation("LightArray["+indexString+"].color");
        glUniform3fv(loc,1,glm::value_ptr(light->color));

        loc = _shader->getUniformLocation("LightArray["+indexString+"].attenuation");
        glUniform3fv(loc,1,glm::value_ptr(light->attenuation));

        loc = _shader->getUniformLocation("LightArray["+indexString+"].angle");
        glUniform1f(loc,light->angle);

        ++index;
        cn = cn->nextNode;
    }

    ///pass model matrix to depth shader
    glUniformMatrix4fv (_depthShader->getUniformLocation("modelMatrix"),    1, GL_FALSE, glm::value_ptr(pGameObject->getWorldTransform() ));

    //pass in all MVP matrices separately
    glUniformMatrix4fv ( _shader->getUniformLocation("projectionMatrix"),   1, GL_FALSE, glm::value_ptr(pCamera->getProjection()));
    glUniformMatrix4fv ( _shader->getUniformLocation("viewMatrix"),         1, GL_FALSE, glm::value_ptr(glm::inverse(pCamera->getWorldTransform())));
    glUniformMatrix4fv ( _shader->getUniformLocation("modelMatrix"),        1, GL_FALSE, glm::value_ptr(pGameObject->getWorldTransform() ) );

    //now inform mesh of where to stream its data
    pGameObject->getMesh()->streamToOpenGL(
        _shader->getAttribLocation("vertex"),
        _shader->getAttribLocation("normal"),
        _shader->getAttribLocation("uv")
    );
}
