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
        TextureLitMaterial (Texture* pDiffuseTexture, Texture * pNormalMapTexture);
        virtual ~TextureLitMaterial ();
        virtual void render(World* pWorld, GameObject* pGameObject, Camera* pCamera);

        void setDiffuseTexture (Texture* pDiffuseTexture);
        void setNormalMapTexture (Texture* pNormalMapTexture);

    protected:
    private:
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();

        Texture* _diffuseTexture;
        Texture* _normalMapTexture;
};

#endif // TextureLitMaterial_H
