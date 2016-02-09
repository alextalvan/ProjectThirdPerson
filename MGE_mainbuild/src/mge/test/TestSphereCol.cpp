#include<mge/test/TestSphereCol.hpp>
#include<mge/core/GameObject.hpp>
#include<mge/util/Input.hpp>
#include<iostream>
#include<glm.hpp>
#include <conio.h>

//bool debug = false;

namespace Test
{
    TestSphereCol::TestSphereCol()
    {
        //OnCollision = &CollisionCallback;
        OnCollision = [this](Collider* other, CollisionMTV mtv) { CollisionCallback(other, mtv);};
        //SetActive(false);
    }

    void TestSphereCol::CollisionCallback(Collider* other, CollisionMTV mtv)
    {

        glm::vec3 relativeDir = _owner->getWorldPosition() - other->getOwner()->getWorldPosition();

        if(glm::dot(relativeDir,mtv.axis)<0)
            mtv.axis *= -1;

        //if(mtv.magnitude > 1.0f)
        //    mtv.magnitude = 2.0f - mtv.magnitude;


        //std::cout<<" - collision depth: "<<mtv.magnitude<<" axis: "<<mtv.axis<<"\n";

        //if(Input::GetKeyDown(Input::D))
        //    debug = !debug;

        //if(debug)
         //getch();
        //if(Input::GetKeyUp(Input::A))
        {
            //_owner->translate(mtv.axis * mtv.magnitude * 1.05f);
            glm::mat4 newtransform = _owner->getWorldTransform();
            glm::vec4 loc = newtransform[3];
            loc += glm::vec4(mtv.axis * mtv.magnitude * 1.05f,0);
            loc.y = 0;
            newtransform[3] = loc;
            _owner->setWorldTransform(newtransform);
        }


    }

    void TestSphereCol::Update()
    {
        //if(Input::GetKeyDown(Input::D))
        //    std::cout<<"\n\nnew\n\n";

        //_owner->translate(glm::vec3(0.0f,0,0.05f));
        //std::cout<<"\n";
        //std::cout<<_owner->getWorldTransform()* glm::vec4(0,0,1,0)<<" "<<_owner->getWorldPosition()<<"\n";
    }
}
