#include "mge/core/Renderer.hpp"
#include <cassert>
#include <GL/glew.h>

#include "mge/core/GameObject.hpp"
#include "mge/core/World.hpp"
#include "mge/core/Camera.hpp"
#include "mge/materials/AbstractMaterial.hpp"

#include <iostream>
using namespace std;

GLuint Renderer::_postProcessVertexAttributeArray;

Renderer::Renderer(int width, int height) : _screenWidth(width), _screenHeight(height)
{
	glEnable( GL_DEPTH_TEST );
	//glEnable( GL_CULL_FACE ); // default GL_BACK
	//glCullFace(GL_FRONT);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor((float)0x2d/0xff, (float)0x6b/0xff, (float)0xce/0xff, 1.0f );

	InitializePostProc();

    //_postProcessList.push_back(new PostProcess("PostProcessing/identity.vs","PostProcessing/cut_screen.fs"));
    //_postProcessList.push_back(new PostProcess("PostProcessing/identity.vs","PostProcessing/identity.fs"));
	//_postProcessList.push_back(new PostProcess("PostProcessing/identity.vs","PostProcessing/cut_screen.fs"));
}

Renderer::~Renderer()
{
    glDeleteRenderbuffers(1, &postProc_rbo_depth);
    glDeleteTextures(1, &postProc_fbo_texture0);
    glDeleteTextures(1, &postProc_fbo_texture1);
    glDeleteFramebuffers(1, &postProc_fbo);
}

void Renderer::setClearColor(int pR, int pG, int pB) {
    glClearColor((float)pR/0xff, (float)pG/0xff, (float)pB/0xff, 1.0f );
}




void Renderer::render (World* pWorld)
{
    if(_postProcessList.size()>0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER,postProc_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProc_fbo_texture0, 0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );//clear previous frame
        render (pWorld, pWorld, pWorld->getMainCamera(), true);//scene pass

        //post processing
        DoPostProcessing();
    }
    else
    {
       glBindFramebuffer(GL_FRAMEBUFFER,0);
       glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
       render (pWorld, pWorld, pWorld->getMainCamera(), true);
    }
}

//note, this function assumes there is at least one post process in the list
void Renderer::DoPostProcessing()
{
    glDisable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_DEPTH_TEST);

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
    glEnable(GL_FRAMEBUFFER_SRGB);
}



void Renderer::render (World* pWorld, GameObject * pGameObject, Camera * pCamera, bool pRecursive)
{
    glEnable(GL_FRAMEBUFFER_SRGB);
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
    if (childCount < 1) return;

    //note that with a loop like this, deleting children during rendering is not a good idea :)
    for (int i = 0; i < childCount; i++) {
        render (pWorld, pGameObject->GetChildAt(i), pCamera, pRecursive);
    }
    glDisable(GL_FRAMEBUFFER_SRGB);
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
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _screenWidth, _screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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

