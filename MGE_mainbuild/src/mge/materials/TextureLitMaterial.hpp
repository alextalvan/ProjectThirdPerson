#ifndef TextureLitMaterial_H
#define TextureLitMaterial_H

#include "mge/core/Texture.hpp"
#include "mge/materials/AbstractMaterial.hpp"
#include "mge/shadow/ShadowCamera.hpp"

/**
 * Simple single texture material, this is a sample which doesn't cache anything upfront and
 * passes in separate matrices for the MVP calculation
 */
class TextureLitMaterial : public AbstractMaterial
{
    public:
        TextureLitMaterial (Texture* pDiffuseTexture, float pSmoothness = 0.5f, float pShininess = 32.0f, float pAmbient = 0.1f,
                            Texture * pNormalMapTexture = nullptr, Texture* pSpecularMapTexture = nullptr, float pTiling = 1.0f,
                            glm::vec4 pColor = glm::vec4(1));
        virtual ~TextureLitMaterial ();
        virtual void render(GameObject* pGameObject, Camera* pCamera);
        void setDiffuseTexture (Texture* pDiffuseTexture);
        void setNormalMapTexture (Texture* pNormalMapTexture);
        void setSpecularMapTexture (Texture* pSpecularMapTexture);
    protected:
    private:
        bool specularMap = false;
        bool normalMap = false;
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();

        static GLint _projMatLoc;
        static GLint _viewMatLoc;
        static GLint _modelMatLoc;
        static GLint _lightMatLoc;

        static GLint _diffMapLoc;
        static GLint _normalMapLoc;
        static GLint _specMapLoc;
        static GLint _depthMapLoc;
        static GLint _tilingLoc;

        static GLint _viewPosLoc;

        static GLint _matSmoothLoc;
        static GLint _matShineLoc;
        static GLint _matAmbLoc;

        static GLboolean _hasSpecMapLoc;
        static GLboolean _hasNormMapLoc;

        static GLuint _colorLoc;

        Texture* _diffuseTexture;
        float _smoothness;
        float _shininess;
        float _ambient;
        Texture* _normalMapTexture;
        Texture* _specularMapTexture;
        float _tiling;
};

#endif // TextureLitMaterial_H
