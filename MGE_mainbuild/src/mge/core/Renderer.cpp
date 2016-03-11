#include "mge/core/Renderer.hpp"
#include <cassert>
#include <GL/glew.h>

#include "mge/core/GameObject.hpp"
#include "mge/core/World.hpp"
#include "mge/core/Camera.hpp"
#include "mge/materials/AbstractMaterial.hpp"
#include "mge/core/Light.hpp"
#include "mge/materials/ShadowMaterial.hpp"
#include "mge/core/Texture.hpp"
#include <SFML/Graphics.hpp>
#include "mge/config.hpp"
#include "mge/core/ShaderProgram.hpp"
#include "mge/core/Time.hpp"
#include "mge/util/list/DualLinkList.hpp"
#include "mge/util/list/DualLinkNode.hpp"
#include "mge/core/Mesh.hpp"

#include <iostream>
using namespace std;

GLuint Renderer::_postProcessVertexAttributeArray;
GLuint Renderer::shadowDepthTextureFar, Renderer::shadowDepthTextureNear, Renderer::shadowDepthTextureMid;

RendererDebugInfo Renderer::debugInfo;

int Renderer::_screenWidth;
int Renderer::_screenHeight;

DualLinkList<TransparencyList> Renderer::transparentList;

ShadowCamera* Renderer::_shadowCam;

glm::mat4 Renderer::nearShadowOrtho = glm::ortho(-20.0f,20.0f,-20.0f,20.0f,0.1f,200.0f);//45+60(neartest) is good
glm::mat4 Renderer::farShadowOrtho = glm::ortho(-1000.0f,1000.0f,-1000.0f,1000.0f,0.1f,200.0f);
glm::mat4 Renderer::midShadowOrtho = glm::ortho(-125.0f,125.0f,-125.0f,125.0f,0.1f,200.0f);
glm::mat4 Renderer::currentShadowOrtho;

Renderer::Renderer(int width, int height)
{
    _screenWidth = width;
    _screenHeight = height;

    //_shadowCam = new ShadowCamera();

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE ); // default GL_BACK
	//glCullFace(GL_FRONT);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor((float)0x2d/0xff, (float)0x6b/0xff, (float)0xce/0xff, 1.0f );

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    InitializeSkyBox();
    InitializeDepthMap();
	InitializePostProc();

	//std::cout<<"\n"<<sf::Texture::getMaximumSize();
	//getchar();

    //_postProcessList.push_back(new PostProcess("PostProcessing/hdr_shader.vs","PostProcessing/hdr_shader.fs"));
    //_postProcessList.push_back(new PostProcess("PostProcessing/identity.vs","PostProcessing/identity.fs"));
	//_postProcessList.push_back(new PostProcess("PostProcessing/identity.vs","PostProcessing/cut_screen.fs"));
	//_postProcessList.push_back(new PostProcess("PostProcessing/identity.vs","PostProcessing/blur.fs"));
}

Renderer::~Renderer()
{
    glDeleteRenderbuffers(1, &postProc_rbo_depth);
    glDeleteRenderbuffers(1, &depthMapFBO);
    glDeleteRenderbuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteTextures(1, &postProc_fbo_texture0);
    glDeleteTextures(1, &postProc_fbo_texture1);
    glDeleteTextures(1, &shadowDepthTextureFar);
    glDeleteTextures(1, &shadowDepthTextureNear);
    glDeleteTextures(1, &shadowDepthTextureMid);
    glDeleteFramebuffers(1, &postProc_fbo);
}

glm::vec2 Renderer::GetScreenSize()
{
    return glm::vec2(_screenWidth,_screenHeight);
}

glm::mat4& Renderer::GetCurrentShadowOrtho()
{
    return currentShadowOrtho;
}

glm::mat4& Renderer::GetNearShadowOrtho()
{
    return nearShadowOrtho;
}

glm::mat4& Renderer::GetFarShadowOrtho()
{
    return farShadowOrtho;
}

glm::mat4& Renderer::GetMidShadowOrtho()
{
    return midShadowOrtho;
}

void Renderer::setClearColor(int pR, int pG, int pB) {
    glClearColor((float)pR/0xff, (float)pG/0xff, (float)pB/0xff, 1.0f );
}

GLuint Renderer::getShadowDepthMapFar()
{
    return shadowDepthTextureFar;
}

GLuint Renderer::getShadowDepthMapNear()
{
    return shadowDepthTextureNear;
}

GLuint Renderer::getShadowDepthMapMid()
{
    return shadowDepthTextureMid;
}

void Renderer::render (World* pWorld)
{
    debugInfo.drawCallCount = debugInfo.triangleCount = 0;
    Time::update();
    float startTime = Time::now();

    Camera* mainCam = pWorld->getMainCamera();

    //used for frustrum culling
    mainCam->RecalculateFrustumCache();

    renderDirLightDepthMap(pWorld);
    //renderPointLightDepthCubeMap(pWorld);

    if(_postProcessList.size()>0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER,postProc_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProc_fbo_texture0, 0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );//clear previous frame
        renderSkyBox(pWorld);
        render (pWorld, mainCam, true);//normal objects pass
        renderTransparentObjects(mainCam);//transparent objects pass


        //post processing
        DoPostProcessing();
    }
    else
    {
       glBindFramebuffer(GL_FRAMEBUFFER,0);
       glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
       renderSkyBox(pWorld);
       render (pWorld, mainCam, true);
       renderTransparentObjects(mainCam);//transparent objects pass

    }

    Time::update();
    debugInfo.time = (Time::now() - startTime) * 1000.0f;
}

void Renderer::renderSkyBox(World* pWorld)
{
    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
    _skyBoxShader->use();
    glm::mat4 view = glm::mat4(glm::mat3(pWorld->getMainCamera()->getView()));	// Remove any translation component of the view matrix
    glm::mat4 projection = pWorld->getMainCamera()->getProjection();//glm::perspective(glm::radians(60.0f), (float)_screenWidth/(float)_screenHeight, 0.1f, 100.0f);
    glUniformMatrix4fv(_skyBoxShader->getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(_skyBoxShader->getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // skybox cube
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(skyboxVAO);
    glUniform1i(_skyBoxShader->getUniformLocation("skybox"), 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // Set depth function back to default
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glCullFace(GL_BACK);
}

void Renderer::renderDirLightDepthMap (World* pWorld)
{
    Light::RecalculateDirLightViewMatrix();
    glViewport(0, 0, 4096, 4096);
    glCullFace(GL_FRONT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    //test version, first clear textures
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureFar, 0);
    glClear(GL_DEPTH_BUFFER_BIT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureNear, 0);
    glClear(GL_DEPTH_BUFFER_BIT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureMid, 0);
    glClear(GL_DEPTH_BUFFER_BIT);


    //first shadow pass - far objects
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureFar, 0);
    //currentShadowOrtho = farShadowOrtho;
    //glClear(GL_DEPTH_BUFFER_BIT);
    renderDepthMap (pWorld,pWorld->getMainCamera(), Light::GetDirectionalLight(), true);

    //glViewport(0, 0, 4096, 4096);
    //second shadow pass - near objects
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureNear, 0);
    //currentShadowOrtho = nearShadowOrtho;
    //glClear(GL_DEPTH_BUFFER_BIT);
    //renderDepthMap (pWorld,pWorld->getMainCamera(), Light::GetDirectionalLight(), true);


    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _screenWidth, _screenHeight);
}

//note, this function assumes there is at least one post process in the list
void Renderer::DoPostProcessing()
{
    //glDisable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_DEPTH_TEST);
    //glCullFace(GL_BACK);

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
    //glCullFace(GL_FRONT);
    //glEnable(GL_FRAMEBUFFER_SRGB);
}

void Renderer::renderDepthMap (GameObject * pGameObject, Camera* pCamera, Light * light, bool pRecursive)
{
    //we don't render inactive gameobjects
    if(!pGameObject->IsActive())
        return;

    if (pGameObject->getMesh() && pGameObject->castShadows )//&& pCamera->FrustumCheck(pGameObject))
    {
        if(ShadowFrustumCheckEncasing(pGameObject,20.0f))//near check
        //if(ShadowFrustumCheckExclusive(pGameObject,10.0f))//near check
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureNear, 0);
            currentShadowOrtho = nearShadowOrtho;
            shadowMat->render(pGameObject, light);
        }
        else
            if(ShadowFrustumCheckExclusive(pGameObject,100.0f))
            //if(ShadowFrustumCheckEncasing(pGameObject,125.0f))//mid check
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureMid, 0);
                currentShadowOrtho = midShadowOrtho;
                shadowMat->render(pGameObject, light);
            }
            else
                if(ShadowFrustumCheckExclusive(pGameObject,1000.0f))//far check
                {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureFar, 0);
                    currentShadowOrtho = farShadowOrtho;
                    shadowMat->render(pGameObject, light);
                }


    }

    if (!pRecursive) return;
    //if (childCount < 1) return;

    DualLinkNode2<ChildList>* cn2 = pGameObject->GetChildren().startNode;
    while(cn2!=NULL)
    {
        renderDepthMap ((GameObject*)cn2,pCamera, light, pRecursive);
        cn2 = cn2->nextNode;
    }
    //for (int i = 0; i < childCount; i++) {
    //    renderDepthMap (pWorld, pGameObject->GetChildAt(i), light, type, pRecursive);
    //}
}

bool Renderer::ShadowFrustumCheckExclusive(GameObject* obj, float orthoSize)
{
    using namespace glm;
    Light* mainLight = Light::GetDirectionalLight();

    mat3 rot = mainLight->getWorldRotation();
    vec3 pos = mainLight->getWorldPosition();

    float meshRadius = obj->getMesh()->GetBoundRadius();
    mat4 objMat = obj->getWorldTransform();

    vec3 objPos = obj->getWorldPosition();

    //must test for non uniform scaling
    float maxScale = glm::length(objMat[0]);
    float newScale = glm::length(objMat[1]);
    if(newScale > maxScale) maxScale = newScale;
    newScale = glm::length(objMat[2]);
    if(newScale > maxScale) maxScale = newScale;

    float objRadius = maxScale * meshRadius;

    vec3 lightProj = vec3(dot(rot[0],pos),dot(rot[1],pos),dot(rot[2],pos));

    if(dot(rot[0],objPos) - objRadius > lightProj[0] + orthoSize )
        return false;

    if(dot(rot[0],objPos) + objRadius < lightProj[0] - orthoSize )
        return false;

    if(dot(rot[1],objPos) - objRadius > lightProj[1] + orthoSize )
        return false;

    if(dot(rot[1],objPos) + objRadius < lightProj[1] - orthoSize )
        return false;

    if(dot(rot[2],objPos) - objRadius > lightProj[2] + 200.0f )
        return false;

    if(dot(rot[2],objPos) + objRadius < lightProj[2] + 0.1f )
        return false;

    return true;
}


bool Renderer::ShadowFrustumCheckEncasing(GameObject* obj, float orthoSize)
{
    using namespace glm;
    Light* mainLight = Light::GetDirectionalLight();

    mat3 rot = mainLight->getWorldRotation();
    vec3 pos = mainLight->getWorldPosition();

    float meshRadius = obj->getMesh()->GetBoundRadius();
    mat4 objMat = obj->getWorldTransform();

    vec3 objPos = obj->getWorldPosition();

    //must test for non uniform scaling
    float maxScale = glm::length(objMat[0]);
    float newScale = glm::length(objMat[1]);
    if(newScale > maxScale) maxScale = newScale;
    newScale = glm::length(objMat[2]);
    if(newScale > maxScale) maxScale = newScale;

    float objRadius = maxScale * meshRadius;

    vec3 lightProj = vec3(dot(rot[0],pos),dot(rot[1],pos),dot(rot[2],pos));

    if(dot(rot[0],objPos) - objRadius > lightProj[0] - orthoSize &&
       dot(rot[0],objPos) + objRadius < lightProj[0] + orthoSize &&
       dot(rot[1],objPos) - objRadius > lightProj[1] - orthoSize &&
       dot(rot[1],objPos) + objRadius < lightProj[1] + orthoSize &&
       dot(rot[2],objPos) - objRadius > lightProj[2] + 0.1f &&
       dot(rot[2],objPos) + objRadius < lightProj[2] + 200.0f)
        return true;
    else
        return false;
}

//regular render cycle, transparent objects are ignored
void Renderer::render (GameObject * pGameObject, Camera * pCamera, bool pRecursive)
{
    //glEnable(GL_FRAMEBUFFER_SRGB);
    //we don't render inactive gameobjects
    if(!pGameObject->IsActive())
        return;

    AbstractMaterial* material = pGameObject->getMaterial();

    //our material (shader + settings) determines how we actually look
    if (!pGameObject->isTransaprent && pGameObject->getMesh() && material != NULL && pCamera->FrustumCheck(pGameObject))
    {
        material->render(pGameObject, pCamera);
    }

    if (!pRecursive) return;

    //int childCount = pGameObject->GetChildCount();
    //if (childCount < 1) return;

    DualLinkNode2<ChildList>* cn2 = pGameObject->GetChildren().startNode;
    while(cn2!=NULL)
    {
        render ((GameObject*)cn2, pCamera, pRecursive);
        cn2 = cn2->nextNode;
    }
    //for (int i = 0; i < childCount; i++) {
    //    render (pWorld, pGameObject->GetChildAt(i), pCamera, pRecursive);
    //}
   // glDisable(GL_FRAMEBUFFER_SRGB);
}

void Renderer::renderTransparentObjects (Camera * pCamera)
{
    glDepthMask(GL_FALSE);//disable depth writing

    DualLinkNode<TransparencyList>* cn = transparentList.startNode;

    while(cn!=NULL)
    {
        GameObject* pGameObject = (GameObject*)cn;
        //glEnable(GL_FRAMEBUFFER_SRGB);
        //we don't render inactive gameobjects
        if(!pGameObject->IsActiveInWorld())
        {
            cn = cn->nextNode;
            continue;
        }


        AbstractMaterial* material = pGameObject->getMaterial();

        if (pGameObject->getMesh() && material != NULL && pCamera->FrustumCheck(pGameObject))
        {
            material->render(pGameObject, pCamera);
        }

        cn = cn->nextNode;

    }

    glDepthMask(GL_TRUE);
}

void Renderer::InitializeSkyBox()
{
    _skyBoxShader = new ShaderProgram();
    _skyBoxShader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH + "skybox_shader.vs");
    _skyBoxShader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH + "skybox_shader.fs");
    _skyBoxShader->finalize();

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
    faces.push_back("Skybox/right.png");
    faces.push_back("Skybox/left.png");
    faces.push_back("Skybox/top.png");
    faces.push_back("Skybox/bottom.png");
    faces.push_back("Skybox/back.png");
    faces.push_back("Skybox/front.png");
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

void Renderer::InitializeDepthMap()
{
      shadowMat = new ShadowMaterial();

      ///depth map (shadow mapping)
      //generate depth map texture
      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &shadowDepthTextureFar);
      glBindTexture(GL_TEXTURE_2D, shadowDepthTextureFar);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
      glBindTexture(GL_TEXTURE_2D, 0);

      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &shadowDepthTextureNear);
      glBindTexture(GL_TEXTURE_2D, shadowDepthTextureNear);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
      glBindTexture(GL_TEXTURE_2D, 0);

      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &shadowDepthTextureMid);
      glBindTexture(GL_TEXTURE_2D, shadowDepthTextureMid);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
      glBindTexture(GL_TEXTURE_2D, 0);

      ///depth buffer (shadow mapping)
      //attach depth map texture to depth buffer
      glGenFramebuffers(1, &depthMapFBO);
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureFar, 0);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _screenWidth, _screenHeight);
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

ShadowCamera* Renderer::GetShadowCamera()
{
    return _shadowCam;
}

