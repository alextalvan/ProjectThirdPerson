#ifndef SHADOWCUBEMATERIAL_HPP
#define SHADOWCUBEMATERIAL_HPP

#include "mge/core/ShaderProgram.hpp"

class World;
class Light;
class GameObject;
class ShaderProgram;

class ShadowCubeMaterial
{
    public:
        ShadowCubeMaterial (int pScreenWidth, int pScreenHeight);
        virtual ~ShadowCubeMaterial ();
        virtual void render(World* pWorld, GameObject* pGameObject, Light* light);
    protected:
    private:
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();

        static GLint _modelMatLoc;
        int _screenWidth, _screenHeight;
};

#endif // SHADOWCUBEMATERIAL_HPP
