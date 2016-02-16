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


namespace LevelEditor
{
    using namespace std;
    World* _storedWorld;
    sf::RenderWindow * _window;

    void LoadLevel(std::string fileName, World* world, sf::RenderWindow * window)
    {


        if(world == NULL)
            return;

        _storedWorld = world;
        _window = window;

        //first release the previous level
        world->DestroyChildren();

        //open the file stream
        ifstream f(fileName);

        //begin by parsing the first element, which is the root object
        ParseAbstractObject(f,world);


        //std::cout<<world->GetChildAt(0)->GetChildAt(1)->getWorldTransform();


        f.close();
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
        parent->AddChild(obj);

        f>>s;//"transform:"

        float val;
        glm::mat4 mat;
        for(int i=0;i<4;++i)
            for(int j=0;j<4;++j)
            {
                f>>s;
                val = std::strtof(s.c_str(),nullptr);
                mat[j][i] = val;
            }

        obj->setWorldTransform(mat);

        //mesh
        f>>s;
        if(s == "mesh")
        {
            f>>s;//read the mesh filename
            Mesh* mesh = Mesh::load(config::MGE_MODEL_PATH + s);
            obj->setMesh(mesh);
        }

        ParseComponents(f,obj);
        ParseChildren(f,obj);
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
                float m,j,g;
                f>>s; m = std::strtof(s.c_str(),nullptr);
                f>>s;//"jump_force"
                f>>s; j = std::strtof(s.c_str(),nullptr);
                f>>s;//"gravity"
                f>>s; g = std::strtof(s.c_str(),nullptr);

                CharacterController* c = new CharacterController(m,j,g);
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

                LuaScript* script = new LuaScript((config::MGE_SCRIPT_PATH + s).c_str(),_storedWorld, _storedWorld->getGui());
                owner->AttachComponent(script);
                f>>s;//end_luascript
            }

            if(s=="SPHERE_COLLIDER:")
            {
                //f>>s;//"name"
                //f>>s;//the name
                f>>s;//"radius"
                float r;
                f>>s; r = std::strtof(s.c_str(),nullptr);
                SphereCollider* col = new SphereCollider();
                col->radius = r;
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
        parent->AddChild(obj);

        f>>s;//"transform:"

        float val;
        glm::mat4 mat;
        for(int i=0;i<4;++i)
            for(int j=0;j<4;++j)
            {
                f>>s;
                val = std::strtof(s.c_str(),nullptr);
                mat[j][i] = val;
            }

        obj->setWorldTransform(mat);

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

        Light* obj = new Light();
        obj->type = MGE_LIGHT_DIRECTIONAL;
        parent->AddChild(obj);

        f>>s;//"transform:"

        float val;
        glm::mat4 mat;
        for(int i=0;i<4;++i)
            for(int j=0;j<4;++j)
            {
                f>>s;
                val = std::strtof(s.c_str(),nullptr);
                mat[j][i] = val;
            }

        obj->setWorldTransform(mat);

        //color
        f>>s;//"color"
        float r,g,b;
        f>>s; r = std::strtof(s.c_str(),nullptr);
        f>>s; g = std::strtof(s.c_str(),nullptr);
        f>>s; b = std::strtof(s.c_str(),nullptr);

        obj->color = glm::vec3(r,g,b);

        f>>s;//"direction"
        f>>s; r = std::strtof(s.c_str(),nullptr);
        f>>s; g = std::strtof(s.c_str(),nullptr);
        f>>s; b = std::strtof(s.c_str(),nullptr);
        obj->direction = glm::vec3(r,g,b);


        ParseComponents(f,obj);
        ParseChildren(f,obj);
        f>>s;//END_LIGHT
    }






}
