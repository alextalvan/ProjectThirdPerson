#ifndef RENDERER_H
#define RENDERER_H

#include<GL/glew.h>
#include "glm.hpp"
#include<vector>
#include<mge/materials/PostProcessing/PostProcess.hpp>
#include<mge/shadow/ShadowCamera.hpp>
#include<mge/util/list/DualLinkList.hpp>
#include<mge/util/list/DualLinkNode.hpp>

//#include
class World;
class GameObject;
class Camera;
class Light;
class ShadowMaterial;
class ShadowCubeMaterial;
class ShaderProgram;
class TransparencyList;

struct RendererDebugInfo
{
    int drawCallCount = 0;
    int triangleCount = 0;
    float time;
};

/**
 * Renderer implements a default single pass forward renderer.
 */
class Renderer
{
	public:
		Renderer(int width, int height);
		virtual ~Renderer();

        //render whole world
        void render (World* pWorld);

        //render specific game object within the world (using world's light settings etc)
        void render (GameObject* pGameObject, Camera* pCamera, bool pRecursive);
        void renderDepthMap (GameObject * pGameObject, Camera* pCamera, Light * light, bool pRecursive);
        //utility call
        void setClearColor (int pR, int pG, int pB);

        static GLuint GetPostProcessVertexAttrib();
        static GLuint getShadowDepthMapFar();
        static GLuint getShadowDepthMapNear();
        static GLuint getShadowDepthMapMid();
        //framebuffer object used for post processing
        static RendererDebugInfo debugInfo;

        static glm::vec2 GetScreenSize();

        //transparency
        static DualLinkList<TransparencyList> transparentList;
        void renderTransparentObjects (Camera * pCamera);

        //test
        static ShadowCamera* GetShadowCamera();

        static glm::mat4& GetCurrentShadowOrtho();
        static glm::mat4& GetFarShadowOrtho();
        static glm::mat4& GetNearShadowOrtho();
        static glm::mat4& GetMidShadowOrtho();

        static bool ShadowFrustumCheckExclusive(GameObject* obj, float orthoSize);
        static bool ShadowFrustumCheckEncasing(GameObject* obj, float orthoSize);
    private:
        //general settings
        static int _screenWidth, _screenHeight;

        ///skybox
        ShaderProgram * _skyBoxShader;
        ///shadow mapping
        static GLuint shadowDepthTextureFar, shadowDepthTextureNear, shadowDepthTextureMid;
        static glm::mat4 nearShadowOrtho, farShadowOrtho, currentShadowOrtho, midShadowOrtho;

        GLuint depthMapFBO;
        // Setup skybox VAO
        GLuint skyboxVAO, skyboxVBO;
        GLuint cubemapTexture;

        //test
        static ShadowCamera* _shadowCam;


        ///post processing
        GLuint postProc_fbo, postProc_fbo_texture0,postProc_fbo_texture1, postProc_rbo_depth;//frame buffer object with depth buffer

        void InitializeSkyBox();
        void InitializeDepthMap();
        void InitializePostProc();

        void renderSkyBox(World* pWorld);
        void renderDirLightDepthMap(World* World);

        ShadowMaterial * shadowMat;

        //vertex attribute array for the post processing
        static GLuint _postProcessVertexAttributeArray;

        std::vector<PostProcess*> _postProcessList;
        void DoPostProcessing();

        GLuint loadCubemap(vector<const GLchar*> faces);


};

#endif // RENDERER_H
