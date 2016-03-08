#ifndef TerrainMaterial_H
#define TerrainMaterial_H

#include "mge/core/Texture.hpp"
#include "mge/materials/AbstractMaterial.hpp"

/**
 * Simple single texture material, this is a sample which doesn't cache anything upfront and
 * passes in separate matrices for the MVP calculation
 */
class TerrainMaterial : public AbstractMaterial
{
    public:
        TerrainMaterial (Texture* height,Texture* splat,Texture* diff0,Texture* diff1,Texture* diff2,Texture* diff3);
        virtual ~TerrainMaterial ();
        virtual void render(GameObject* pGameObject, Camera* pCamera);

    protected:
    private:
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();

        Texture* _heightMap;
        Texture* _splatMap;

        Texture* _diffuse0;
        Texture* _diffuse1;
        Texture* _diffuse2;
        Texture* _diffuse3;
};

#endif // TerrainMaterial_H
