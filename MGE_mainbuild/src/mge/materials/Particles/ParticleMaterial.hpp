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
        virtual void render(World* pWorld, GameObject* pGameObject, Camera* pCamera);

        void setDiffuseTexture (Texture* pDiffuseTexture);

    protected:
    private:
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();

        Texture* _diffuseTexture;
        GLuint _vpMatrixLoc;
        GLuint _rotMatrixLoc;
        GLuint _posVectorLoc;
        GLuint _scaleVectorLoc;
        GLuint _vertLoc;
        GLuint _normalLoc;
        GLuint _uvLoc;
};

