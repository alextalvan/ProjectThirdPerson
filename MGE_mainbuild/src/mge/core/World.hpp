#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "mge/core/GameObject.hpp"

class Light;
class Camera;

class World : public GameObject
{
	public:
        World();

		static void setMainCamera (Camera* pCamera);
		static Camera* getMainCamera();
        void renderDebugInfo();

	private:
	    static Camera* _mainCamera;

};


#endif // WORLD_H
