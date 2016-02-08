#ifndef COLORMATERIAL_H
#define COLORMATERIAL_H

#include "mge/materials/AbstractMaterial.hpp"
#include <glm.hpp>

/**
 * Simple single color material.
 * This material uses no lights.
 */
class ColorMaterial : public AbstractMaterial
{
    public:
        ColorMaterial(glm::vec3 pColor = glm::vec3(1,0,0));
        virtual ~ColorMaterial();
        virtual void render(World* pWorld, GameObject* pGameObject, Camera* pCamera);

        //in rgb values
        void setDiffuseColor (glm::vec3 pDiffuseColor);

    private:
        //all the static properties are shared between instances of ColorMaterial
        //note that they are all PRIVATE, we do not expose this static info to the outside world
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();

        //in this example we cache all identifiers for uniforms & attributes
        static GLint _uMVPMatrix;
        static GLint _uDiffuseColor;

        static GLint _aVertex ;
        static GLint _aNormal;
        static GLint _aUV ;

        //this one is unique per instance of color material
        glm::vec3 _diffuseColor;
};

#endif // COLORMATERIAL_H
