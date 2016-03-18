#pragma once
#include <mge/materials/AbstractMaterial.hpp>
#include <glm.hpp>
#include "mge/core/Texture.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/config.hpp"


class ParticleMaterial : public AbstractMaterial
{
    public:
        ParticleMaterial (Texture* pDiffuseTexture);
        virtual ~ParticleMaterial ();
        virtual void render(GameObject* pGameObject, Camera* pCamera);

        void setDiffuseTexture (Texture* pDiffuseTexture);

    protected:
    private:
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();

        Texture* _diffuseTexture;
        static GLuint _vpMatrixLoc;
        static GLuint _rotMatrixLoc;
        static GLuint _posLoc;
        static GLuint _lifetimeLoc;
        static GLuint _scaleLoc;
        static GLuint _texLoc;

        static GLuint _offsetsBuffer;
        static GLuint _lifetimesBuffer;
        static GLuint _scalesBuffer;
};

