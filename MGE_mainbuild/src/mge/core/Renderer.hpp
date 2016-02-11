#ifndef RENDERER_H
#define RENDERER_H

#include<GL/glew.h>
#include<vector>
//#include
class World;
class GameObject;
class Camera;

#include<mge/materials/PostProcessing/PostProcess.hpp>

/**
 * Renderer implements a default single pass forward renderer.
 */
class Renderer
{
	public:
		Renderer();
		virtual ~Renderer();

        //render whole world
        void render (World* pWorld);

        //render specific game object within the world (using world's light settings etc)
        void render (World* pWorld, GameObject* pGameObject, Camera* pCamera, bool pRecursive);

        //utility call
        void setClearColor (int pR, int pG, int pB);

        static GLuint GetPostProcessVertexAttrib();

    //framebuffer object used for post processing
    private:
        ///shadow mapping
        const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 768;
        GLuint depthMapFBO, depthMap;

        ///post processing
        GLuint postProc_fbo, postProc_fbo_texture0,postProc_fbo_texture1, postProc_rbo_depth;//frame buffer object with depth buffer
        void InitializePostProc();

        //vertex attribute array for the post processing
        static GLuint _postProcessVertexAttributeArray;


        //one single post processing material for now, replace with list and optimize later
        std::vector<PostProcess*> _postProcessList;

        void DoPostProcessing();

        //PostProcess* _postProcessProgram;

};

#endif // RENDERER_H
