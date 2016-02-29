#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include "mge/core/GameObject.hpp"

/**
 * Camera is just a GameObject with an additional perspective matrix.
 * The camera's own transform is used to generate a world to view matrix by taking the inverse of the camera transform.
 */

 struct FrustrumCheckCache
 {
     //glm::vec3 camPos, nearPos, farPos;
     glm::vec3 leftNorm, rightNorm, upNorm, downNorm, backNorm, forwardNorm;
     float leftOrigin, rightOrigin, upOrigin, downOrigin, backOrigin, forwardOrigin;
 };

class Camera : public GameObject
{
	public:
		Camera(
            std::string pName = "camera",
            glm::vec3 pPosition = glm::vec3( 0,0,0 ),
            glm::mat4 pProjectionMatrix = glm::perspective (glm::radians(60.0f), 4.0f/3.0f, 0.1f, 1000.0f  )
        );

		virtual ~Camera();

        glm::mat4& getProjection();
        glm::mat4& getView();

        void RecalculateFrustumCache();
        bool FrustumCheck(GameObject* obj);
    protected:
        virtual void MakeTransformDirty() override;
        virtual void _recalculateLocalTransform() override;

	private:
		glm::mat4 _projection;

		//view matrix is cached instead of recalculated every time
		glm::mat4 _view;
		bool _viewMatrixDirty = true;


		//frustrum culling
		FrustrumCheckCache _cullingCache;

};

#endif // CAMERA_H
