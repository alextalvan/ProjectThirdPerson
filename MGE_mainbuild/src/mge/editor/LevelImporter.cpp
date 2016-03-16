#include <mge/editor/LevelImporter.hpp>
#include <mge/config.hpp>
#include <glm.hpp>
#include <mge/materials/ColorMaterial.hpp>
#include <mge/core/Mesh.hpp>
#include <mge/core/Texture.hpp>
#include <mge/materials/TextureMaterial.hpp>
#include <mge/materials/TextureLitMaterial.hpp>
#include <mge/collision/BoxCollider.hpp>
#include <mge/collision/WallCollider.hpp>
#include <mge/core/Camera.hpp>
#include <mge/core/Light.hpp>
#include <mge/behaviours/CharacterController.hpp>
#include <mge/lua/LuaScript.hpp>
#include <mge/gui/GUI.hpp>
#include <mge/particles/ParticleSystem.hpp>
#include <mge/collision/TerrainCollider.hpp>
#include <mge/behaviours/FirstPersonLook.hpp>
#include <vector>


namespace LevelEditor
{
    using namespace std;
    World* _storedWorld;
    GUI* _storedWorld2D;
    //dummy class
    std::vector<LuaScript*> _storedScripts;

    void LoadLevel(std::string fileName, World* world, GUI* world2D)
    {
        if(world == NULL || world2D == NULL)
            return;

        _storedScripts.clear();

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

        if(s == "PARTICLE_SYSTEM:")
        {
            ParseParticleSystem(f,parent);
            return true;
        }

        if(s == "POINT_LIGHT:")
        {
            ParsePointLight(f,parent);
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

        //shadow toggle
        f>>s;//"cast_shadows"
        int readValue;
        f>>readValue;
        obj->castShadows = readValue;

        f>>s;//"transparent"
        f>>readValue;

        if(readValue)
            obj->SetTransparent(true);

        f>>s;//"ignore non directional lights"
        f>>readValue;
        obj->ignoreNonDirLights = readValue;

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
                string name;
                f>>s;//"diffuseTex
                f>>s;//the filename

                Texture* tex = Texture::load(config::MGE_TEXTURE_PATH + s);
                TextureMaterial* mat1 = new TextureMaterial(tex);

                f>>s;//"color
                float r,g,b,a;
                f>>s; r = std::strtof(s.c_str(),nullptr);
                f>>s; g = std::strtof(s.c_str(),nullptr);
                f>>s; b = std::strtof(s.c_str(),nullptr);
                f>>s; a = std::strtof(s.c_str(),nullptr);

                mat1->color = glm::vec4(r,g,b,a);


                owner->setMaterial(mat1);
                f>>s;//end_texmat
            }

            if(s=="TEXTURE_ADVANCED_MATERIAL:")
            {
                //f>>s;//"name"
                //f>>s;//the name
                f>>s;//"diffuseTex
                f>>s;//the filename

                Texture* diffuse = nullptr;
                if(s!="missing")
                diffuse = Texture::load(config::MGE_TEXTURE_PATH + s);

                f>>s;//"normalTex
                f>>s;//the filename

                Texture* normalMap = nullptr;
                if(s!="missing")
                normalMap = Texture::load(config::MGE_TEXTURE_PATH + s);

                f>>s;//"specularTex
                f>>s;//the filename

                Texture* specularMap = nullptr;
                if(s!="missing")
                specularMap = Texture::load(config::MGE_TEXTURE_PATH + s);

                float smooth,shiny,ambient;
                float difTile,normTile,specTile;

                f>>s;
                f>>s; smooth = std::strtof(s.c_str(),nullptr);
                f>>s;
                f>>s; shiny = std::strtof(s.c_str(),nullptr);
                f>>s;
                f>>s; ambient = std::strtof(s.c_str(),nullptr);
                f>>s;
                f>>s; difTile = std::strtof(s.c_str(),nullptr);
                f>>s;
                f>>s; normTile = std::strtof(s.c_str(),nullptr);
                f>>s;
                f>>s; specTile = std::strtof(s.c_str(),nullptr);

                //std::cout<<"\n"<<smooth<<" "<<shiny<<" "<<ambient;


                TextureLitMaterial* mat2 = new TextureLitMaterial(diffuse,smooth,shiny,ambient,normalMap,specularMap);

                mat2->SetDiffuseTiling(difTile);
                mat2->SetNormalTiling(normTile);
                mat2->SetSpecularTiling(specTile);

                f>>s;//"color
                float r,g,b,a;
                f>>s; r = std::strtof(s.c_str(),nullptr);
                f>>s; g = std::strtof(s.c_str(),nullptr);
                f>>s; b = std::strtof(s.c_str(),nullptr);
                f>>s; a = std::strtof(s.c_str(),nullptr);

                mat2->color = glm::vec4(r,g,b,a);

                owner->setMaterial(mat2);
                f>>s;//end_texmat
            }

            if(s=="COLOR_MATERIAL:")
            {
                //f>>s;//"name"
                //f>>s;//the name
                f>>s;//"color
                float r,g,b,a;
                f>>s; r = std::strtof(s.c_str(),nullptr);
                f>>s; g = std::strtof(s.c_str(),nullptr);
                f>>s; b = std::strtof(s.c_str(),nullptr);
                f>>s; a = std::strtof(s.c_str(),nullptr);
                ColorMaterial* mat2 = new ColorMaterial(glm::vec4(r,g,b,a));
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
                f>>s;//"collision_layer"
                int layer;
                f>>layer;
                box->layer = (CollisionManager::COLLISION_LAYERS)layer;

                f>>s;//"is static"
                f>>layer;
                box->SetStatic(layer);

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

                int igX,igY,igZ;
                f>>s;//"ig axis"
                f>>igX;
                f>>s;//"ig axis"
                f>>igY;
                f>>s;//"ig axis"
                f>>igZ;

                WallCollider* box = new WallCollider();
                box->xSize = r; box->ySize = g; box->zSize = b;
                box->ignoreXaxis = igX;
                box->ignoreYaxis = igY;
                box->ignoreZaxis = igZ;

                f>>s;//"is_static"
                f>>igX;
                box->SetStatic(igX);

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

                //hard coding this shit

                //glm::vec3 lightDir = glm::vec3(1, -1.5f, 1);
                //float n = 300.0f;
                //glm::vec3 lightPos = glm::normalize(lightDir) * -n; //+ glm::vec3(0,5,0); //+ glm::vec3(n*1.25f,0,n*1.25f);
                //Light* light1 = new Light(MGE_LIGHT_DIRECTIONAL, lightPos, lightDir , glm::vec3(1.0f,0.5f,0.188f),glm::vec3(0.1f),0,owner);
                //_storedWorld->AddChild(light1);
                f>>s;//end_character
            }

            if(s=="LUA_SCRIPT:")
            {
                //string name;
                //f>>s;//"name"
                //f>>name;//the name

                f>>s;//"script_name"
                f>>s;//the actual script path

                LuaScript* script = new LuaScript(s,_storedWorld, _storedWorld2D);
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
                f>>s;//"collision_layer"
                int layer;
                f>>layer;


                SphereCollider* col = new SphereCollider(r,ig);
                col->layer = (CollisionManager::COLLISION_LAYERS)layer;

                f>>s;//"is static"
                f>>layer;
                col->SetStatic(layer);


                owner->AttachComponent(col);
                f>>s;//end_sphere
            }

            if(s=="TERRAIN_COLLIDER:")
            {
                std::string heightMap;
                f>>s;//heightTex
                f>>heightMap;//the filename

                //Texture* tex = Texture::load(config::MGE_TEXTURE_PATH + s);

                float x,z,h;
                f>>s;f>>s; x = std::strtof(s.c_str(),nullptr);
                f>>s;f>>s; z = std::strtof(s.c_str(),nullptr);
                f>>s;f>>s; h = std::strtof(s.c_str(),nullptr);

                TerrainCollider* col = new TerrainCollider(heightMap,x,z,h);
                owner->AttachComponent(col);

                f>>s;//end_terrain
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

    void ParseParticleSystem(ifstream& f, GameObject* parent)
    {
        string s,name;

        f>>s;//"name"
        f>>name;//and the actual name

        f>>s;//"diffuseTex
        f>>s;//the filename

        Texture* tex = Texture::load(config::MGE_TEXTURE_PATH + s);

        ParticleSystem* pSys = new ParticleSystem(tex);
        pSys->setName(name);

        ProcessModelMatrix(f,parent,pSys);

        glm::vec3 spawnRange1, spawnRange2, speedMin, speedMax;
        glm::vec2 lifeRange, delayRange, scaleRange,releaseCount;
        int limit;


        f>>s;//"spawn range 1
        f>>s; spawnRange1.x = std::strtof(s.c_str(),nullptr); f>>s; spawnRange1.y = std::strtof(s.c_str(),nullptr); f>>s; spawnRange1.z = std::strtof(s.c_str(),nullptr);

        f>>s;//"spawn range 2
        f>>s; spawnRange2.x = std::strtof(s.c_str(),nullptr); f>>s; spawnRange2.y = std::strtof(s.c_str(),nullptr); f>>s; spawnRange2.z = std::strtof(s.c_str(),nullptr);

        f>>s;//"speed min
        f>>s; speedMin.x = std::strtof(s.c_str(),nullptr); f>>s; speedMin.y = std::strtof(s.c_str(),nullptr); f>>s; speedMin.z = std::strtof(s.c_str(),nullptr);

        f>>s;//"speed max
        f>>s; speedMax.x = std::strtof(s.c_str(),nullptr); f>>s; speedMax.y = std::strtof(s.c_str(),nullptr); f>>s; speedMax.z = std::strtof(s.c_str(),nullptr);

        f>>s;//"liferange
        f>>s; lifeRange.x = std::strtof(s.c_str(),nullptr); f>>s; lifeRange.y = std::strtof(s.c_str(),nullptr);

        f>>s;//"delayRange
        f>>s; delayRange.x = std::strtof(s.c_str(),nullptr); f>>s; delayRange.y = std::strtof(s.c_str(),nullptr);

        f>>s;//"releaseCount
        f>>s; releaseCount.x = std::strtof(s.c_str(),nullptr); f>>s; releaseCount.y = std::strtof(s.c_str(),nullptr);

        f>>s;//"scaleRange
        f>>s; scaleRange.x = std::strtof(s.c_str(),nullptr); f>>s; scaleRange.y = std::strtof(s.c_str(),nullptr);

        f>>s;//particle_limit
        f>>limit;

        pSys->startOffset1 = spawnRange1;
        pSys->startOffset2 = spawnRange2;
        pSys->speedMin = speedMin;
        pSys->speedMax = speedMax;
        pSys->lifeTimeRange = lifeRange;
        pSys->releaseDelay = delayRange;
        pSys->releaseCount = releaseCount;
        pSys->scaleRange = scaleRange;
        pSys->SetParticleLimit(limit);

        ParseComponents(f,pSys);
        ParseChildren(f,pSys);
        f>>s;//END_PSYS
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

        f>>s;//attach first person
        f>>isMain;

        if(isMain)
        {
            obj->AttachComponent(new FirstPersonLook());
        }

        ParseComponents(f,obj);
        ParseChildren(f,obj);
        f>>s;//END_CAMERA
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
            //mat[3][2] *= -1.0f;

            mat = glm::transpose(mat);


            //mat[3][2] *= -1.0f;


             //mat[3][2] *= -1.0f;


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
        //child->scale(glm::vec3(-1,1,1));

    }


     void ParsePointLight(ifstream& f, GameObject* parent)
    {
        string s,name;
        f>>s;//"name"
        f>>name;//and the actual name

        //GameObject* obj = new GameObject(s,glm::vec3(0));
        Light* light = new Light(MGE_LIGHT_POINT);
        light->setName(name);

        ProcessModelMatrix(f,parent,light);


        float r,g,b,a1,a2,a3;
        f>>s;//"color"
        f>>s; r = std::strtof(s.c_str(),nullptr);
        f>>s; g = std::strtof(s.c_str(),nullptr);
        f>>s; b = std::strtof(s.c_str(),nullptr);

        f>>s;//attenuation
        f>>s; a1 = std::strtof(s.c_str(),nullptr);
        f>>s; a2 = std::strtof(s.c_str(),nullptr);
        f>>s; a3 = std::strtof(s.c_str(),nullptr);

        light->setColor(glm::vec3(r,g,b));
        light->setAttenuation(glm::vec3(a1,a2,a3));

        f>>s;//falloff
        f>>s; a1 = std::strtof(s.c_str(),nullptr);
        light->setFalloff(a1);


        ParseComponents(f,light);
        ParseChildren(f,light);

        f>>s;//END_POINTLIGHT


    }



}
