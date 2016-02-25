#include "mge/core/Renderer.hpp"
#include <cassert>
#include <GL/glew.h>

#include "mge/core/GameObject.hpp"
#include "mge/core/World.hpp"
#include "mge/core/Camera.hpp"
#include "mge/materials/AbstractMaterial.hpp"
#include "mge/util/list/DualLinkNode.hpp"
#include "mge/core/Light.hpp"
#include "mge/materials/ShadowMaterial.hpp"
#include "mge/materials/ShadowCubeMaterial.hpp"
#include "mge/core/Texture.hpp"
#include <SFML/Graphics.hpp>
#include "mge/config.hpp"
#include "mge/core/ShaderProgram.hpp"

#include <iostream>
using namespace std;

GLuint Renderer::_postProcessVertexAttributeArray;
GLuint Renderer::depthMap;
GLuint Renderer::depthCubeMap;


RendererDebugInfo Renderer::debugInfo;

Renderer::Renderer(int width, int height) : _screenWidth(width), _screenHeight(height)
{
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE ); // default GL_BACK
	glCullFace(GL_FRONT);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor((float)0x2d/0xff, (float)0x6b/0xff, (float)0xce/0xff, 1.0f );

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    InitializeSkyBox();
    InitializeDepthMaps();
	InitializePostProc();

    //_postProcessList.push_back(new PostProcess("PostProcessing/hdr_shader.vs","PostProcessing/hdr_shader.fs"));
    _postProcessList.push_back(new PostProcess("PostProcessing/identity.vs","PostProcessing/identity.fs"));
	//_postProcessList.push_back(new PostProcess("PostProcessing/identity.vs","PostProcessing/cut_screen.fs"));
}

Renderer::~Renderer()
{
    glDeleteRenderbuffers(1, &postProc_rbo_depth);
    glDeleteRenderbuffers(1, &depthMapFBO);
    glDeleteRenderbuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    //glDeleteRenderbuffers(1, &depthCubeMapFBO);
    glDeleteTextures(1, &postProc_fbo_texture0);
    glDeleteTextures(1, &postProc_fbo_texture1);
    glDeleteTextures(1, &depthMap);
    glDeleteTextures(1, &cubemapTexture);
    //glDeleteTextures(1, &depthCubeMap);
    glDeleteFramebuffers(1, &postProc_fbo);
}

void Renderer::setClearColor(int pR, int pG, int pB) {
    glClearColor((float)pR/0xff, (float)pG/0xff, (float)pB/0xff, 1.0f );
}

GLuint Renderer::getDepthMap()
{
    return depthMap;
}

GLuint Renderer::getDepthCubeMap()
{
    return depthCubeMap;
}

void Renderer::render (World* pWorld)
{
    debugInfo.drawCallCount = debugInfo.triangleCount = 0;

    renderDirLightDepthMap(pWorld);
    //renderPointLightDepthCubeMap(pWorld);

    if(_postProcessList.size()>0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER,postProc_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProc_fbo_texture0, 0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );//clear previous frame
        render (pWorld, pWorld, pWorld->getMainCamera(), true);//scene pass

        renderSkyBox(pWorld);
        //post processing
        DoPostProcessing();
    }
    else
    {
       glBindFramebuffer(GL_FRAMEBUFFER,0);
       glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
       render (pWorld, pWorld, pWorld->getMainCamera(), true);
        renderSkyBox(pWorld);
    }

}

void Renderer::renderSkyBox(World* pWorld)
{
    glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
    _shader->use();
    glm::mat4 view = glm::mat4(glm::mat3(pWorld->getMainCamera()->getView()));	// Remove any translation component of the view matrix
    glm::mat4 projection = pWorld->getMainCamera()->getProjection();//glm::perspective(glm::radians(60.0f), (float)_screenWidth/(float)_screenHeight, 0.1f, 100.0f);
    glUniformMatrix4fv(_shader->getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(_shader->getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // skybox cube
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(skyboxVAO);
    glUniform1i(_shader->getUniformLocation("skybox"), 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // Set depth function back to default
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Renderer::renderDirLightDepthMap (World* pWorld)
{
    DualLinkNode<Light>* l = Light::GetLightList().startNode;
    while (l != NULL) {
            Light* light = (Light*)l;
        if (light->getType() == MGE_LIGHT_DIRECTIONAL) {
            ///shadow mapping (render depth map)
            glViewport(0, 0, _screenWidth, _screenHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_BACK);
            renderDepthMap (pWorld, pWorld, light, MGE_LIGHT_DIRECTIONAL, true);
            glCullFace(GL_FRONT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, _screenWidth, _screenHeight);
        }
        l = l -> nextNode;
    }
}

void Renderer::renderPointLightDepthCubeMap (World* pWorld)
{
    DualLinkNode<Light>* l = Light::GetLightList().startNode;
    while (l != NULL) {
            Light* light = (Light*)l;
        if (light->getType() == MGE_LIGHT_POINT) {
            ///shadow mapping (render depth cube map)
            glViewport(0, 0, _screenWidth, _screenHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_BACK);
            renderDepthMap (pWorld, pWorld, light, MGE_LIGHT_POINT, true);
            glCullFace(GL_FRONT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, _screenWidth, _screenHeight);
        }
        l = l -> nextNode;
    }
}

//note, this function assumes there is at least one post process in the list
void Renderer::DoPostProcessing()
{
    //glDisable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    int procListSize = _postProcessList.size();
    //setup
    bool firstTextureIsSource = true;//first time the game scene was rendered into tex0, so using tex0 as source first


    //actual rendering, all post processes except the last one render to our special postproc frame buffer object
    //the two textures are used as double buffer for the frame so it is not overwritten after each render
    for(int i=0;i<procListSize;++i)
    {
        //swap textures
        GLuint currentSourceTexture,currentOutputTexture;

        if(firstTextureIsSource)
        {
            currentSourceTexture = postProc_fbo_texture0;
            currentOutputTexture = postProc_fbo_texture1;
        }
        else
        {
            currentSourceTexture = postProc_fbo_texture1;
            currentOutputTexture = postProc_fbo_texture0;
        }
        firstTextureIsSource = !firstTextureIsSource;


        if(i==procListSize-1)
        {
            glBindFramebuffer(GL_FRAMEBUFFER,0);
        }
        else
        {
             //bind the output texture as the render target of the buffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, currentOutputTexture, 0);
        }

        glClear( GL_COLOR_BUFFER_BIT );
        //post process from the current texture source
        _postProcessList[i]->render(currentSourceTexture);



    }
    glEnable( GL_DEPTH_TEST );
    glCullFace(GL_FRONT);
    //glEnable(GL_FRAMEBUFFER_SRGB);
}

void Renderer::renderDepthMap (World* pWorld, GameObject * pGameObject, Light * light, int type, bool pRecursive)
{
    //we don't render inactive gameobjects
    if(!pGameObject->IsActive())
        return;

    if (type == MGE_LIGHT_DIRECTIONAL) {

        if (pGameObject->getMesh() && pGameObject->castShadows) {
            shadowMat->render(pWorld, pGameObject, light);
        }
    }
    /*
    else if (type == MGE_LIGHT_POINT) {
        //our material (shader + settings) determines how we actually look
        if (pGameObject->getMesh() && shadowCubeMat != NULL) {
            shadowCubeMat->render(pWorld, pGameObject, light);
        }
    }*/

    if (!pRecursive) return;

    int childCount = pGameObject->GetChildCount();
    //if (childCount < 1) return;

    //note that with a loop like this, deleting children during rendering is not a good idea :)
    for (int i = 0; i < childCount; i++) {
        renderDepthMap (pWorld, pGameObject->GetChildAt(i), light, type, pRecursive);
    }
}

void Renderer::render (World* pWorld, GameObject * pGameObject, Camera * pCamera, bool pRecursive)
{
    //glEnable(GL_FRAMEBUFFER_SRGB);
    //we don't render inactive gameobjects
    if(!pGameObject->IsActive())
        return;

    AbstractMaterial* material = pGameObject->getMaterial();

    //our material (shader + settings) determines how we actually look
    if (pGameObject->getMesh() && material != NULL) {
        material->render(pWorld, pGameObject, pCamera);
    }

    if (!pRecursive) return;

    int childCount = pGameObject->GetChildCount();
    //if (childCount < 1) return;

    //note that with a loop like this, deleting children during rendering is not a good idea :)
    for (int i = 0; i < childCount; i++) {
        render (pWorld, pGameObject->GetChildAt(i), pCamera, pRecursive);
    }
   // glDisable(GL_FRAMEBUFFER_SRGB);
}

void Renderer::InitializeSkyBox()
{
    _shader = new ShaderProgram();
    _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH + "skybox_shader.vs");
    _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH + "skybox_shader.fs");
    _shader->finalize();

    GLfloat skyboxVertices[] = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    vector<const GLchar*> faces;
    faces.push_back("right.jpg");
    faces.push_back("left.jpg");
    faces.push_back("top.jpg");
    faces.push_back("bottom.jpg");
    faces.push_back("back.jpg");
    faces.push_back("front.jpg");
    cubemapTexture = loadCubemap(faces);
}

GLuint Renderer::loadCubemap(vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    sf::Image image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for(GLuint i = 0; i < faces.size(); i++)
    {
        if (image.loadFromFile(config::MGE_TEXTURE_PATH + faces[i])) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
                         image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

void Renderer::InitializeDepthMaps()
{
      shadowMat = new ShadowMaterial();
      //shadowCubeMat = new ShadowCubeMaterial(_screenWidth, _screenHeight);

      ///depth map (shadow mapping)
      //generate depth map texture
      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &depthMap);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _screenWidth, _screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
      glBindTexture(GL_TEXTURE_2D, 0);

      ///depth buffer (shadow mapping)
      //attach depth map texture to depth buffer
      glGenFramebuffers(1, &depthMapFBO);
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

/*
      ///depth cube map (omnidirectional shadow mapping)
      //generate depth cube map
      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &depthCubeMap);
      glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
      for (GLuint i = 0; i < 6; ++i)
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, _screenWidth, _screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

      ///depth buffer (omnidirectional shadow mapping)
      //attach depth cube map texture to depth buffer
      glGenFramebuffers(1, &depthCubeMapFBO);
      glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
*/
}

void Renderer::InitializePostProc()
{
    /* Create back-buffer, used for post-processing */

      /* Textures, 2 are used in order to enable multiple sequential post process effects */
      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &postProc_fbo_texture0);
      glBindTexture(GL_TEXTURE_2D, postProc_fbo_texture0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _screenWidth, _screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
      glBindTexture(GL_TEXTURE_2D, 0);

      //texture1
      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &postProc_fbo_texture1);
      glBindTexture(GL_TEXTURE_2D, postProc_fbo_texture1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _screenWidth, _screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      glBindTexture(GL_TEXTURE_2D, 0);

      /* Depth buffer */
      glGenRenderbuffers(1, &postProc_rbo_depth);
      glBindRenderbuffer(GL_RENDERBUFFER, postProc_rbo_depth);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _screenWidth, _screenHeight);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);

      /* Framebuffer to link everything together */
      glGenFramebuffers(1, &postProc_fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, postProc_fbo);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProc_fbo_texture0, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, postProc_rbo_depth);
      GLenum status;
      if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout<<"PostProcess Framebuffer generation failure!\n";
        return;
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //vertex array containing NDC verts
      GLfloat fbo_vertices[] = {
        -1, -1,
         1, -1,
        -1,  1,
         1,  1,
      };
      glGenBuffers(1, &_postProcessVertexAttributeArray);
      glBindBuffer(GL_ARRAY_BUFFER, _postProcessVertexAttributeArray);
      glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      //_postProcessProgram = new PostProcess(postProc_fbo_texture0,_postProcessVertexAttributeArray);
}

GLuint Renderer::GetPostProcessVertexAttrib()
{
    return _postProcessVertexAttributeArray;
}

