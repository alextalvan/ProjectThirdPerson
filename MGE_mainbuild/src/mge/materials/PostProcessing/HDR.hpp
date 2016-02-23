#pragma once

#include <GL/glew.h>
#include <mge/core/ShaderProgram.hpp>
#include <string>

/**
 * Simple single texture material, this is a sample which doesn't cache anything upfront and
 * passes in separate matrices for the MVP calculation
 */
class HDR
{
    public:
        HDR ();
        virtual ~HDR ();
        virtual void render(GLuint frameTextureID);

        bool enabled = true;

    protected:
    private:
        ShaderProgram* _shader;
        void InitializeShader();

        //GLuint frameTexture;
        //GLuint vertexArray;

         //program id and uniforms locations for post processing
        //GLuint program_postproc, attribute_v_coord_postproc, uniform_fbo_texture;

};
