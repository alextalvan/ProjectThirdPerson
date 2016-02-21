#ifndef SHADOWMATERIAL_HPP
#define SHADOWMATERIAL_HPP

#include "mge/core/ShaderProgram.hpp"

class World;
class Light;
class GameObject;
class ShaderProgram;

class ShadowMaterial
{
    public:
        ShadowMaterial ();
        virtual ~ShadowMaterial ();
        virtual void render(World* pWorld, GameObject* pGameObject, Light* light);
    protected:
    private:
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();

        static GLint _lightMatLoc;
        static GLint _modelMatLoc;
};

#endif // SHADOWMATERIAL_HPP
