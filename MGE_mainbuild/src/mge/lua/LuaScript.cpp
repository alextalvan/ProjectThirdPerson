#include "mge/lua/LuaScript.hpp"
#include "mge/core/World.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/materials/ColorMaterial.hpp"
#include "mge/materials/TextureMaterial.hpp"
#include "mge/behaviours/KeysBehaviour.hpp"
//#include "mge/behaviours/OrbitBehaviour.hpp"
#include "mge/behaviours/LookAt.hpp"
//#include "WobblingMaterial.hpp"
//#include "LitColorMaterial.hpp"
//#include "LitTextureMaterial.hpp"
#include "mge/config.hpp"
#include "mge/util/Input.hpp"
#include "mge/util/Random.hpp"
#include "mge/core/Time.hpp"
#include "mge/collision/SphereCollider.hpp"
#include "mge/collision/BoxCollider.hpp"
#include "mge/collision/WallCollider.hpp"

//#define MGE_LUA_SAFETY 1  //comment this define out to remove the lua safety checks but heavily improve performance


LuaScript::LuaScript(const char * path, World * world)
{

    _name = "LuaScript:" + std::string(path);
	//Open lua
	L = lua_open();
	//Open libs
	luaL_openlibs(L);

	//Register functions
	lua_register(L, "LoadMesh", loadMesh);//
	lua_register(L, "LoadTexture", loadTexture);//
	lua_register(L, "ColorMaterial", colorMaterial);//
	lua_register(L, "TextureMaterial", textureMaterial);//
	lua_register(L, "GameObject", gameObject);//
	lua_register(L, "SetName", setName);//
	lua_register(L, "GetName", getName);//
	lua_register(L, "SetMesh", setMesh);//
	lua_register(L, "GetMesh", getMesh);//
	lua_register(L, "SetMaterial", setMaterial);//
	lua_register(L, "GetMaterial", getMaterial);//
	lua_register(L, "Camera", camera);//
	lua_register(L, "AddToWorld", addToWorld);//
	lua_register(L, "SetMainCamera", setMainCamera);//
	lua_register(L, "SetParent", setParent);//
	lua_register(L, "GetParent", getParent);//
	lua_register(L, "GetChildCount", getChildCount);
	lua_register(L, "GetChildAt", getChildAt);
	lua_register(L, "SetLocalPos", setLocalPosition);//
	lua_register(L, "GetWorldPos", getWorldPos);//
	lua_register(L, "GetLocalPos", getLocalPos);//
	lua_register(L, "Rotate", rotate);//
	lua_register(L, "Scale", scale);//
	lua_register(L, "Translate", translate);//
	lua_register(L, "GetTime", getTime);//
	lua_register(L, "GetKey", keyPressed);//
	lua_register(L, "GetKeyUp", keyUp);//
	lua_register(L, "GetKeyDown", keyDown);//
	lua_register(L, "GetMouseButton", mouseButton);//
	lua_register(L, "GetMouseButtonUp", mouseButtonUp);//
	lua_register(L, "GetMousePosition", mousePosition);//
	lua_register(L, "AttachComponent", attachComponent);//
	lua_register(L, "Distance", distance);//
	lua_register(L, "RandomInRange", randomRange);//
	lua_register(L, "RollDice", randomRoll);//
	lua_register(L, "SphereCollider", sphereCollider);//
	lua_register(L, "BoxCollider", boxCollider);//
	lua_register(L, "WallCollider", wallCollider);//
	lua_register(L, "LuaScript", luaScript);//

	//Set world
	lua_pushlightuserdata(L, world);
	lua_setglobal(L, "world");

	//set self reference
	lua_pushlightuserdata(L, this);
	lua_setglobal(L, "this");

    //Load file
	luaL_dofile(L, path);
	//Start
	lua_getglobal(L, "Start");
	lua_call(L, 0, 0);
}

void LuaScript::setOwner(GameObject* pOwner)
{
    Component::setOwner(pOwner);
    if(pOwner==NULL)
    {
        lua_pushnil(L);
        lua_setglobal(L,"myGameObject");
    }
    else
    {
        lua_pushlightuserdata(L, pOwner);
        lua_setglobal(L,"myGameObject");

        lua_getglobal(L, "OnAttach");
        lua_call(L, 0, 0);
    }
}

int LuaScript::loadMesh(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, lua_gettop(lua))) throw "Expect: string";
	#endif

	string fileName = lua_tostring(lua, lua_gettop(lua));


	Mesh* mesh = Mesh::load(config::MGE_MODEL_PATH + fileName);
	lua_pushlightuserdata(lua, mesh);

	return 1;
}

int LuaScript::loadTexture(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, lua_gettop(lua))) throw "Expect: string";
	#endif
	string fileName = lua_tostring(lua, lua_gettop(lua));

	Texture * texture = Texture::load(config::MGE_TEXTURE_PATH + fileName);
	lua_pushlightuserdata(lua, texture);

	return 1;
}

int LuaScript::colorMaterial(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	glm::vec3 color = glm::vec3(0, 0, 0);
	color.x = lua_tonumber(lua, -3);
	color.y = lua_tonumber(lua, -2);
	color.z = lua_tonumber(lua, -1);

	AbstractMaterial* newMaterial = new ColorMaterial(color);
	lua_pushlightuserdata(lua, newMaterial);

	return 1;
}

int LuaScript::textureMaterial(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: texture";
	#endif

	Texture * texture = (Texture*)lua_touserdata(lua, lua_gettop(lua));

	AbstractMaterial* newMaterial = new TextureMaterial(texture);
	lua_pushlightuserdata(lua, newMaterial);

	return 1;
}

int LuaScript::gameObject(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, -4)) throw "Expect: string";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	string name = lua_tostring(lua, -4);
	glm::vec3 position = glm::vec3(0, 0, 0);
	position.x = lua_tonumber(lua, -3);
	position.y = lua_tonumber(lua, -2);
	position.z = lua_tonumber(lua, -1);

	GameObject * gameObj = new GameObject(name, position);

	lua_getglobal(lua,"world");
	World* w = (World*) lua_touserdata(lua,-1);
	w->AddChild(gameObj);
	lua_pushlightuserdata(lua, gameObj);

	return 1;
}

int LuaScript::camera(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, -4)) throw "Expect: string";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	string name = lua_tostring(lua, -4);
	glm::vec3 position = glm::vec3(0, 0, 0);
	position.x = lua_tonumber(lua, -3);
	position.y = lua_tonumber(lua, -2);
	position.z = lua_tonumber(lua, -1);

	Camera * cam = new Camera(name, position);
	lua_pushlightuserdata(lua, cam);

	return 1;
}

int LuaScript::addToWorld(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: world";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";
	#endif

	World * world = (World*)lua_touserdata(lua, -2);
	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -1);

	world->AddChild(gameObj);

	return 0;
}

int LuaScript::setMainCamera(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: world";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: camera";
	#endif

	World * world = (World*)lua_touserdata(lua, -2);
	Camera * cam = (Camera*)lua_touserdata(lua, -1);

	world->setMainCamera(cam);

	return 0;
}

int LuaScript::setParent(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";
	#endif

	GameObject * parent = (GameObject*)lua_touserdata(lua, -2);
	GameObject * child = (GameObject*)lua_touserdata(lua, -1);

	parent->AddChild(child);

	return 0;
}

int LuaScript::getParent(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObject = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	GameObject * parent = gameObject->getParent();

	lua_pushlightuserdata(lua, parent);

	return 1;
}

int LuaScript::getChildCount(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObject = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	int count = gameObject->GetChildCount();

	lua_pushnumber(lua, count);

	return 1;
}

int LuaScript::getChildAt(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GameObject * gameObject = (GameObject*)lua_touserdata(lua, -2);
	int index = lua_tonumber(lua, -2);
	GameObject * child = gameObject->GetChildAt(index);

	lua_pushlightuserdata(lua, child);

	return 1;
}

int LuaScript::setLocalPosition(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -4);
	glm::vec3 position = glm::vec3(0, 0, 0);
	position.x = lua_tonumber(lua, -3);
	position.y = lua_tonumber(lua, -2);
	position.z = lua_tonumber(lua, -1);

	gameObj->setLocalPosition(position);

	return 0;
}

int LuaScript::getWorldPos(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));

	glm::vec3 pos = gameObj->getWorldPosition();

	lua_pushnumber(lua, pos.x);
	lua_pushnumber(lua, pos.y);
	lua_pushnumber(lua, pos.z);

	return 3;
}

int LuaScript::getLocalPos(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));

	glm::vec3 pos = gameObj->getLocalPosition();

	lua_pushnumber(lua, pos.x);
	lua_pushnumber(lua, pos.y);
	lua_pushnumber(lua, pos.z);

	return 3;
}

int LuaScript::rotate(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -5)) throw "Expect: game object";
	if (!lua_isnumber(lua, -4)) throw "Expect: number";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -5);
	float angle = lua_tonumber(lua, -4);
	glm::vec3 axis = glm::vec3(0, 0, 0);
	axis.x = lua_tonumber(lua, -3);
	axis.y = lua_tonumber(lua, -2);
	axis.z = lua_tonumber(lua, -1);

	gameObj->rotate(glm::radians(angle), axis);

	return 0;
}

int LuaScript::scale(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -4);
	glm::vec3 scalar = glm::vec3(0, 0, 0);
	scalar.x = lua_tonumber(lua, -3);
	scalar.y = lua_tonumber(lua, -2);
	scalar.z = lua_tonumber(lua, -1);

	gameObj->scale(scalar);

	return 0;
}

int LuaScript::translate(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -4);
	glm::vec3 translation = glm::vec3(0, 0, 0);
	translation.x = lua_tonumber(lua, -3);
	translation.y = lua_tonumber(lua, -2);
	translation.z = lua_tonumber(lua, -1);

	gameObj->translate(translation);

	return 0;
}

int LuaScript::setMesh(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: mesh";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -2);
	Mesh * mesh = (Mesh*)lua_touserdata(lua, -1);

	gameObj->setMesh(mesh);

	return 0;
}

int LuaScript::getMesh(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	Mesh * mesh = gameObj->getMesh();

	lua_pushlightuserdata(lua, mesh);

	return 1;
}

int LuaScript::setMaterial(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: material";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -2);
	AbstractMaterial * material = (AbstractMaterial*)lua_touserdata(lua, -1);

	gameObj->setMaterial(material);

	return 0;
}

int LuaScript::getMaterial(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	AbstractMaterial * material = gameObj->getMaterial();

	lua_pushlightuserdata(lua, material);

	return 1;
}

int LuaScript::setName(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isstring(lua, -1)) throw "Expect: string";
	#endif

	string name = lua_tostring(lua, -1);
	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -2);

	gameObj->setName(name);

	return 0;
}

int LuaScript::getName(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	const char * name = gameObj->getName().c_str();

	lua_pushstring(lua, name);

	return 1;
}

int LuaScript::getTime(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	#endif
	float time = Time::now();
	lua_pushnumber(lua, time);

	return 1;
}

int LuaScript::keyPressed(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";
	#endif

	int keyIndex = lua_tonumber(lua, lua_gettop(lua));
	lua_pushboolean(lua, Input::GetKey((Input::KeyCode)keyIndex));

	return 1;
}

int LuaScript::keyUp(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";
	#endif

	int keyIndex = lua_tonumber(lua, lua_gettop(lua));
	lua_pushboolean(lua, Input::GetKeyUp((Input::KeyCode)keyIndex));

	return 1;
}

int LuaScript::keyDown(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";
	#endif

	int keyIndex = lua_tonumber(lua, lua_gettop(lua));
	lua_pushboolean(lua, Input::GetKeyDown((Input::KeyCode)keyIndex));

	return 1;
}

int LuaScript::mouseButton(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";
	#endif

	int button = lua_tonumber(lua, lua_gettop(lua));
	lua_pushboolean(lua, Input::GetMouseButton(button));

	return 1;
}

int LuaScript::mouseButtonUp(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";
	#endif

	int button = lua_tonumber(lua, lua_gettop(lua));
	lua_pushboolean(lua, Input::GetMouseButtonUp(button));

	return 1;
}

int LuaScript::mousePosition(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	#endif
    lua_pushinteger(lua,Input::getMouseX());
    lua_pushinteger(lua,Input::getMouseY());
	return 2;
}

int LuaScript::randomRange(lua_State * lua)
{
    int r1 = lua_tonumber(lua, -2);
    int r2 = lua_tonumber(lua, -1);

    lua_pushinteger(lua,Utils::Random::GetValue(r1,r2));
	return 1;
}


int LuaScript::randomRoll(lua_State * lua)
{
    int sides = lua_tonumber(lua, -1);
    lua_pushboolean(lua,Utils::Random::Roll(sides));
	return 1;
}



int LuaScript::attachComponent(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY

	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: behaviour";
	#endif

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -2);
	Component * comp = (Component*)lua_touserdata(lua, -1);

	//gameObj->AttachComponent(comp);
	comp->setOwner(gameObj);

	return 0;

}

int LuaScript::sphereCollider(lua_State * lua)
{
    SphereCollider* sphere = new SphereCollider();
    float rad = lua_tonumber(lua, -1);
    sphere->radius = rad;
    lua_pushlightuserdata(lua,sphere);
    return 1;
}

int LuaScript::boxCollider(lua_State * lua)
{
    BoxCollider* box = new BoxCollider();
    float x = lua_tonumber(lua, -3);
    float y = lua_tonumber(lua, -2);
    float z = lua_tonumber(lua, -1);
    box->xSize = x;
    box->ySize = y;
    box->zSize = z;
    lua_pushlightuserdata(lua,box);
    return 1;
}

int LuaScript::wallCollider(lua_State * lua)
{
    WallCollider* wall = new WallCollider();
    float x = lua_tonumber(lua, -3);
    float y = lua_tonumber(lua, -2);
    float z = lua_tonumber(lua, -1);
    wall->xSize = x;
    wall->ySize = y;
    wall->zSize = z;
    lua_pushlightuserdata(lua,wall);
    return 1;
}

int LuaScript::luaScript(lua_State * lua)
{
    string fileName = lua_tostring(lua, -1);
    lua_getglobal(lua,"world");
    World* world = (World*)lua_touserdata(lua,-1);
    LuaScript* script = new LuaScript((config::MGE_SCRIPT_PATH+fileName).c_str(),world);
    lua_pushlightuserdata(lua,script);
    return 1;
}

//removed for now
/*
int LuaScript::getBehaviour(lua_State * lua)
{

	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	AbstractBehaviour * behaviour = gameObj->getBehaviour();

	lua_pushlightuserdata(lua, behaviour);

	return 1;
}
*/

int LuaScript::distance(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";
	#endif

	GameObject * gameObj1 = (GameObject*)lua_touserdata(lua, -2);
	GameObject * gameObj2 = (GameObject*)lua_touserdata(lua, -1);
	glm::vec3 pos1 = gameObj1->getWorldPosition();
	glm::vec3 pos2 = gameObj2->getWorldPosition();
	float distance = glm::distance(pos1, pos2);

	lua_pushnumber(lua, distance);

	return 1;
}

void LuaScript::Update()
{
	lua_getglobal(L, "Update");
	lua_call(L, 0, 0);
	//cout<<_name<<"\n";
}

void LuaScript::InvokeCollisionCallback(GameObject* other)
{
    lua_getglobal(L, "OnCollision");
    lua_pushlightuserdata(L,other);
	lua_call(L, 1, 0);
}

LuaScript::~LuaScript()
{
	lua_close(L);
}
