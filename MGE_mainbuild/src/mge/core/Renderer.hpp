#ifndef RENDERER_H
#define RENDERER_H

#include<GL/glew.h>
#include "glm.hpp"
#include<vector>
#include<mge/materials/PostProcessing/PostProcess.hpp>
#include<mge/shadow/ShadowCamera.hpp>
//#include
class World;
class GameObject;
class Camera;
class Light;
class ShadowMaterial;
class ShadowCubeMaterial;
class ShaderProgram;

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
        void render (World* pWorld, GameObject* pGameObject, Camera* pCamera, bool pRecursive);
        void renderDepthMap (World* pWorld, GameObject * pGameObject, Camera* pCamera, Light * light, int type, bool pRecursive);
        //utility call
        void setClearColor (int pR, int pG, int pB);

        static GLuint GetPostProcessVertexAttrib();
        static GLuint getDepthMap();
        static GLuint getDepthCubeMap();
        //framebuffer object used for post processing
        static RendererDebugInfo debugInfo;

        static glm::vec2 GetScreenSize();

        //test
        static ShadowCamera* GetShadowCamera();
    private:
        //general settings
        static int _screenWidth, _screenHeight;

        ///skybox
        ShaderProgram * _skyBoxShader;
        ///shadow mapping
        static GLuint depthMap;
        static GLuint depthCubeMap;
        GLuint depthMapFBO;
        GLuint depthCubeMapFBO;
        // Setup skybox VAO
        GLuint skyboxVAO, skyboxVBO;
        GLuint cubemapTexture;

        //test
        static ShadowCamera* _shadowCam;


        ///post processing
        GLuint postProc_fbo, postProc_fbo_texture0,postProc_fbo_texture1, postProc_rbo_depth;//frame buffer object with depth buffer

        void InitializeSkyBox();
        void InitializeDepthMaps();
        void InitializePostProc();

        void renderSkyBox(World* pWorld);
        void renderDirLightDepthMap(World* World);
        void renderPointLightDepthCubeMap(World* pWorld);

        ShadowMaterial * shadowMat;
        ShadowCubeMaterial * shadowCubeMat;

        //vertex attribute array for the post processing
        static GLuint _postProcessVertexAttributeArray;


        std::vector<PostProcess*> _postProcessList;
        void DoPostProcessing();

        GLuint loadCubemap(vector<const GLchar*> faces);


};

#endif // RENDERER_H
