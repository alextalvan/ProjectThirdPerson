#ifndef TextureLitMaterial_H
#define TextureLitMaterial_H

#include "mge/core/Texture.hpp"
#include "mge/materials/AbstractMaterial.hpp"

/**
 * Simple single texture material, this is a sample which doesn't cache anything upfront and
 * passes in separate matrices for the MVP calculation
 */
class TextureLitMaterial : public AbstractMaterial
{
    public:
        TextureLitMaterial (Texture* pDiffuseTexture, float pSmoothness = 0.5f, float pShininess = 32.0f, float pAmbient = 0.1f, Texture * pNormalMapTexture = nullptr, Texture* pSpecularMapTexture = nullptr);
        virtual ~TextureLitMaterial ();
        virtual void render(World* pWorld, GameObject* pGameObject, Camera* pCamera);

        void setDiffuseTexture (Texture* pDiffuseTexture);
        void setNormalMapTexture (Texture* pNormalMapTexture);
        void setSpecularMapTexture (Texture* pSpecularMapTexture);

    protected:
    private:
        bool normalMap = false;
        bool specularMap = false;
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();


        Texture* _diffuseTexture;
        Texture* _normalMapTexture;
        Texture* _specularMapTexture;
        float _smoothness;
        float _shininess;
        float _ambient;
};

#endif // TextureLitMaterial_H
