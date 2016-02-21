#ifndef RENDERER_H
#define RENDERER_H

#include<GL/glew.h>
#include<vector>
//#include
class World;
class GameObject;
class Camera;
class Light;

#include<mge/materials/PostProcessing/PostProcess.hpp>

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
        void renderShadowDepth (World* pWorld, GameObject * pGameObject, Light * light, bool pRecursive);
        //utility call
        void setClearColor (int pR, int pG, int pB);

        static GLuint GetPostProcessVertexAttrib();
        static GLuint getDepthMap();
    //framebuffer object used for post processing
    private:
        //general settings
        int _screenWidth, _screenHeight;

        ///shadow mapping
        static GLuint depthMap;
        GLuint depthMapFBO;

        ///post processing
        GLuint postProc_fbo, postProc_fbo_texture0,postProc_fbo_texture1, postProc_rbo_depth;//frame buffer object with depth buffer
        void InitializePostProc();

        //vertex attribute array for the post processing
        static GLuint _postProcessVertexAttributeArray;

        Light* getDirLight();

        //one single post processing material for now, replace with list and optimize later
        std::vector<PostProcess*> _postProcessList;

        void DoPostProcessing();

        //PostProcess* _postProcessProgram;

};

#endif // RENDERER_H
