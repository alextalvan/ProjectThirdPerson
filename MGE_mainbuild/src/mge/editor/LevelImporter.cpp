#include <mge/editor/LevelImporter.hpp>
#include <mge/config.hpp>
#include <glm.hpp>
#include <mge/materials/ColorMaterial.hpp>
#include <mge/core/Mesh.hpp>
#include <mge/core/Texture.hpp>
#include <mge/materials/TextureMaterial.hpp>
#include <mge/collision/BoxCollider.hpp>
#include <mge/collision/WallCollider.hpp>
#include <mge/core/Camera.hpp>
#include <mge/core/Light.hpp>
#include <mge/behaviours/CharacterController.hpp>
#include <mge/lua/LuaScript.hpp>
#include <mge/gui/GUI.hpp>
#include <vector>


namespace LevelEditor
{
    using namespace std;
    World* _storedWorld;
    GUI* _storedWorld2D;
    std::vector<LuaScript*> _storedScripts;

    void LoadLevel(std::string fileName, World* world, GUI* world2D)
    {
        if(world == NULL || world2D == NULL)
            return;

        _storedWorld = world;
        _storedWorld2D = world2D;

        //first release the previous level
        world->DestroyChildren();
        world2D->DestroyChildren();

        std::cout<<"Loading level " + fileName + "...\n";

        //open the file stream
        ifstream f(fileName);

        //begin by parsing the first element, which is the root object
        ParseAbstractObject(f,world);

        f.close();
        std::cout<<"Successfully loaded level " + fileName + "\n";


        //lua level load callbacks
        ((LuaScript*)(world->FindComponent("LuaScript:main.lua")))->InvokeFunction("OnLevelLoad");


        for(unsigned int i=0;i<_storedScripts.size();++i)
        {
            _storedScripts[i]->InvokeFunction("OnLevelLoad");
        }
        _storedScripts.clear();
    }

    bool ParseAbstractObject(ifstream& f, GameObject* parent)
    {
        string s;
        f>>s;

        if(s == "GAMEOBJECT:")
        {
            ParseGameObject(f,parent);
            return true;
        }

        if(s == "CAMERA:")
        {
            ParseCamera(f,parent);
            return true;
        }

        if(s == "DIRECTIONAL_LIGHT:")
        {
            ParseDirLight(f,parent);
            return true;
        }

        return false;
        //f>>s;
        //f>>objectType;//END_
        //if(!(f>>objectType))
        //    return;
    }

    void ParseGameObject(ifstream& f, GameObject* parent)
    {
        string s;
        f>>s;//"name"
        f>>s;//and the actual name

        GameObject* obj = new GameObject(s,glm::vec3(0));


        ProcessModelMatrix(f,parent,obj);

        //mesh
        f>>s;
        if(s == "mesh")
        {
            f>>s;//read the mesh filename
            Mesh* mesh = Mesh::load(config::MGE_MODEL_PATH + s);
            obj->setMesh(mesh);
        }


        //the exporter root is ignored
        if(obj->getName() == "MGE_exporter_root")
        {
            ParseComponents(f,parent);//passing the world
            ParseChildren(f,parent);
            obj->Destroy();
        }
        else
        {
            ParseComponents(f,obj);
            ParseChildren(f,obj);
        }


        f>>s;//END_GAMEOBJECT


    }



    void ParseComponents(ifstream& f, GameObject* owner)
    {
        string s;
        f>>s;//"COMPONENTS:

        f>>s;//here would be first component, if any
        while(s!="END_COMPONENTS")
        {
            if(s=="TEXTURE_MATERIAL:")
            {
                //f>>s;//"name"
                //f>>s;//the name
                f>>s;//"diffuseTex
                f>>s;//the filename

                Texture* tex = Texture::load(config::MGE_TEXTURE_PATH + s);
                TextureMaterial* mat1 = new TextureMaterial(tex);
                owner->setMaterial(mat1);
                f>>s;//end_texmat
            }

            if(s=="COLOR_MATERIAL:")
            {
                //f>>s;//"name"
                //f>>s;//the name
                f>>s;//"color
                float r,g,b;
                f>>s; r = std::strtof(s.c_str(),nullptr);
                f>>s; g = std::strtof(s.c_str(),nullptr);
                f>>s; b = std::strtof(s.c_str(),nullptr);
                ColorMaterial* mat2 = new ColorMaterial(glm::vec3(r,g,b));
                owner->setMaterial(mat2);
                f>>s;//end_colormat
            }

            if(s=="BOX_COLLIDER:")
            {
                //f>>s;//"name"
                //f>>s;//the name
                f>>s;//"dimensions"
                float r,g,b;
                f>>s; r = std::strtof(s.c_str(),nullptr);
                f>>s; g = std::strtof(s.c_str(),nullptr);
                f>>s; b = std::strtof(s.c_str(),nullptr);
                BoxCollider* box = new BoxCollider();
                box->xSize = r; box->ySize = g; box->zSize = b;
                owner->AttachComponent(box);
                f>>s;//end_boxcollider
            }

            if(s=="WALL_COLLIDER:")
            {
                //f>>s;//"name"
                //f>>s;//the name
                f>>s;//"dimensions"
                float r,g,b;
                f>>s; r = std::strtof(s.c_str(),nullptr);
                f>>s; g = std::strtof(s.c_str(),nullptr);
                f>>s; b = std::strtof(s.c_str(),nullptr);
                WallCollider* box = new WallCollider();
                 box->xSize = r; box->ySize = g; box->zSize = b;
                owner->AttachComponent(box);
                f>>s;//end_wall
            }

            if(s=="CHARACTER_CONTROLLER:")
            {
                //f>>s;//"name"
                //f>>s;//the name
                f>>s;//"move_speed"
                float m,j,g,r,d;
                f>>s; m = std::strtof(s.c_str(),nullptr);
                f>>s;//"jump_force"
                f>>s; j = std::strtof(s.c_str(),nullptr);
                f>>s;//"jump_duration"
                f>>s; d = std::strtof(s.c_str(),nullptr);
                f>>s;//"gravity"
                f>>s; g = std::strtof(s.c_str(),nullptr);
                f>>s;//"sphere radius"
                f>>s; r = std::strtof(s.c_str(),nullptr);

                CharacterController* c = new CharacterController(m,j,g,r,d);
                owner->AttachComponent(c);
                f>>s;//end_character
            }

            if(s=="LUA_SCRIPT:")
            {
                //string name;
                //f>>s;//"name"
                //f>>name;//the name

                f>>s;//"script_name"
                f>>s;//the actual script path

                LuaScript* script = new LuaScript(s.c_str(),_storedWorld, _storedWorld2D);
                owner->AttachComponent(script);
                _storedScripts.push_back(script);
                f>>s;//end_luascript
            }

            if(s=="SPHERE_COLLIDER:")
            {
                //f>>s;//"name"
                //f>>s;//the name
                f>>s;//"radius"
                float r; int ig;
                f>>s; r = std::strtof(s.c_str(),nullptr);
                f>>s;//"ignoreRaycast"
                f>>ig;
                SphereCollider* col = new SphereCollider(r,ig);
                owner->AttachComponent(col);
                f>>s;//end_sphere
            }
            f>>s;//new component or end_components
        }
    }


    void ParseChildren(ifstream& f, GameObject* parent)
    {
        string s;
        f>>s;//CHILDREN:

        //f>>s;//first child or
        bool cont = true;

        while(cont)
        {
            cont = ParseAbstractObject(f,parent);
            //f>>s;
        }
    }



    void ParseCamera(ifstream& f, GameObject* parent)
    {
        string s;
        f>>s;//"name"
        f>>s;//and the actual name

        Camera* obj = new Camera(s,glm::vec3(0));

        ProcessModelMatrix(f,parent,obj);


        //mesh
        f>>s;//isMainCamera
        int isMain;
        f>>isMain;//actual value, 1 or 0

        if(isMain) _storedWorld->setMainCamera(obj);

        ParseComponents(f,obj);
        ParseChildren(f,obj);
        f>>s;//END_CAMERA
    }



    void ParseDirLight(ifstream& f, GameObject* parent)
    {
         string s;
        f>>s;//"name"
        f>>s;//and the actual name

        Light* obj = new Light(MGE_LIGHT_DIRECTIONAL);
        //obj->type = MGE_LIGHT_DIRECTIONAL;
        //parent->AddChild(obj);


        ProcessModelMatrix(f,parent,obj);

        //color
        f>>s;//"color"
        float r,g,b;
        f>>s; r = std::strtof(s.c_str(),nullptr);
        f>>s; g = std::strtof(s.c_str(),nullptr);
        f>>s; b = std::strtof(s.c_str(),nullptr);

        obj->setColor(glm::vec3(r,g,b));

        f>>s;//"direction"
        f>>s; r = std::strtof(s.c_str(),nullptr);
        f>>s; g = std::strtof(s.c_str(),nullptr);
        f>>s; b = std::strtof(s.c_str(),nullptr);
        obj->setDirection(glm::vec3(r,g,b));


        ParseComponents(f,obj);
        ParseChildren(f,obj);
        f>>s;//END_LIGHT
    }

    void ProcessModelMatrix(ifstream& f, GameObject* parent, GameObject* child)
    {
        string s;

        f>>s;//"transform:"

        float val;
        glm::mat4 mat;
        for(int i=0;i<4;++i)
            for(int j=0;j<4;++j)
            {
                f>>s;
                val = std::strtof(s.c_str(),nullptr);

                mat[i][j] = val;
            }

            //mat[0] = mat[0] * -1.0f;
            //mat[2] = mat[2] * -1.0f;
            //mat[2] = mat[2] * -1.0f;

            mat[0][0] *= -1.0f;
            mat[1][0] *= -1.0f;
            mat[2][0] *= -1.0f;
            mat[3][2] *= -1.0f;

            mat = glm::transpose(mat);


            //mat[1] = mat[1] * -1.0f;
            //mat[2][2] = mat[2][2] * -1.0f;
        /*
        f>>s;//"transform:"
        f>>s;//"position"

        glm::vec3 pos;
        f>>s; pos.x = std::strtof(s.c_str(),nullptr);
        f>>s; pos.y = std::strtof(s.c_str(),nullptr);
        f>>s; pos.z = std::strtof(s.c_str(),nullptr);

        f>>s;//"rotation"

        glm::quat rot;
        f>>s; rot.x = std::strtof(s.c_str(),nullptr);
        f>>s; rot.y = std::strtof(s.c_str(),nullptr);
        f>>s; rot.z = std::strtof(s.c_str(),nullptr);
        f>>s; rot.w = std::strtof(s.c_str(),nullptr);

        f>>s;//"scale"

        glm::vec3 scale;
        f>>s; scale.x = std::strtof(s.c_str(),nullptr);
        f>>s; scale.y = std::strtof(s.c_str(),nullptr);
        f>>s; scale.z = std::strtof(s.c_str(),nullptr);

        glm::mat4 rotation = glm::mat4_cast(rot);


        glm::mat4 model = glm::translate(pos) * rotation * glm::scale(scale);
        //rotation[0] = rotation[0] * scale.x;
        //rotation[1] = rotation[1] * scale.y;
        //rotation[2] = rotation[2] * scale.z;

        //rotation[3] = glm::vec4(pos,1);
        */

        parent->AddChild(child);
        child->setWorldTransform(mat);

    }




}
