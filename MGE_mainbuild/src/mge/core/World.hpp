#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "mge/core/GameObject.hpp"

class Light;
class Camera;
class GUI;

class World : public GameObject
{
	public:
        World(sf::RenderWindow * pWindow);
		static void setMainCamera (Camera* pCamera);
		static Camera* getMainCamera();
        void renderDebugInfo();
        GUI * getGui();
	private:
	    static Camera* _mainCamera;
        GUI * gui;

};


#endif // WORLD_H
