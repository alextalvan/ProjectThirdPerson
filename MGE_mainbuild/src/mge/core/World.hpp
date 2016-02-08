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

		void setMainCamera (Camera* pCamera);
		Camera* getMainCamera();

        void renderDebugInfo();

	private:
	    Camera* _mainCamera;
};


#endif // WORLD_H
