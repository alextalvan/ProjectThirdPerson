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
#include <sfml/window/event.hpp>

LuaScript::LuaScript(const char * path, World * world)
{
    _name = "LuaScript";
	//Open lua
	L = lua_open();
	//Open libs
	luaL_openlibs(L);

	//Register functions
	lua_register(L, "LoadMesh", loadMesh);
	lua_register(L, "LoadTexture", loadTexture);
	lua_register(L, "ColorMaterial", colorMaterial);
	lua_register(L, "TextureMaterial", textureMaterial);
	lua_register(L, "GameObject", gameObject);
	lua_register(L, "SetName", setName);
	lua_register(L, "GetName", getName);
	lua_register(L, "SetMesh", setMesh);
	lua_register(L, "GetMesh", getMesh);
	lua_register(L, "SetMaterial", setMaterial);
	lua_register(L, "GetMaterial", getMaterial);
	lua_register(L, "Camera", camera);
	lua_register(L, "AddToWorld", addToWorld);
	lua_register(L, "SetMainCamera", setMainCamera);
	lua_register(L, "SetParent", setParent);
	lua_register(L, "GetParent", getParent);
	lua_register(L, "Remove", remove);
	lua_register(L, "GetChildCount", getChildCount);
	lua_register(L, "GetChildAt", getChildAt);
	lua_register(L, "SetLocalPos", setLocalPosition);
	lua_register(L, "GetWorldPos", getWorldPos);
	lua_register(L, "GetLocalPos", getLocalPos);
	lua_register(L, "Rotate", rotate);
	lua_register(L, "Scale", scale);
	lua_register(L, "Translate", translate);
	lua_register(L, "GetTime", getTime);
	lua_register(L, "KeyPressed", keyPressed);
	lua_register(L, "KeysBehaviour", keysBehaviour);
	lua_register(L, "OrbitBehaviour", orbitBehaviour);
	lua_register(L, "LookAtBehaviour", lookAtBehaviour);
	lua_register(L, "SetBehaviour", setBehaviour);
	lua_register(L, "GetBehaviour", getBehaviour);
	lua_register(L, "Distance", distance);

	//Load file
	luaL_dofile(L, path);

	//Set world
	lua_pushlightuserdata(L, world);
	lua_setglobal(L, "World");

	//Start
	lua_getglobal(L, "Start");
	lua_call(L, 0, 0);
}

int LuaScript::loadMesh(lua_State * lua)
{
	if (!lua_isstring(lua, lua_gettop(lua))) throw "Expect: string";
	string fileName = lua_tostring(lua, lua_gettop(lua));

	Mesh* mesh = Mesh::load(config::MGE_MODEL_PATH + fileName);
	lua_pushlightuserdata(lua, mesh);

	return 1;
}

int LuaScript::loadTexture(lua_State * lua)
{
	if (!lua_isstring(lua, lua_gettop(lua))) throw "Expect: string";
	string fileName = lua_tostring(lua, lua_gettop(lua));

	Texture * texture = Texture::load(config::MGE_TEXTURE_PATH + fileName);
	lua_pushlightuserdata(lua, texture);

	return 1;
}

int LuaScript::colorMaterial(lua_State * lua)
{
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";

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
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: texture";

	Texture * texture = (Texture*)lua_touserdata(lua, lua_gettop(lua));

	AbstractMaterial* newMaterial = new TextureMaterial(texture);
	lua_pushlightuserdata(lua, newMaterial);

	return 1;
}

int LuaScript::gameObject(lua_State * lua)
{
	if (!lua_isstring(lua, -4)) throw "Expect: string";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";

	string name = lua_tostring(lua, -4);
	glm::vec3 position = glm::vec3(0, 0, 0);
	position.x = lua_tonumber(lua, -3);
	position.y = lua_tonumber(lua, -2);
	position.z = lua_tonumber(lua, -1);

	GameObject * gameObj = new GameObject(name, position);
	lua_pushlightuserdata(lua, gameObj);

	return 1;
}

int LuaScript::camera(lua_State * lua)
{
	if (!lua_isstring(lua, -4)) throw "Expect: string";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";

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
	if (!lua_islightuserdata(lua, -2)) throw "Expect: world";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";

	World * world = (World*)lua_touserdata(lua, -2);
	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -1);

	world->AddChild(gameObj);

	return 0;
}

int LuaScript::setMainCamera(lua_State * lua)
{
	if (!lua_islightuserdata(lua, -2)) throw "Expect: world";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: camera";

	World * world = (World*)lua_touserdata(lua, -2);
	Camera * cam = (Camera*)lua_touserdata(lua, -1);

	world->setMainCamera(cam);

	return 0;
}

int LuaScript::setParent(lua_State * lua)
{
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";

	GameObject * parent = (GameObject*)lua_touserdata(lua, -2);
	GameObject * child = (GameObject*)lua_touserdata(lua, -1);

	parent->AddChild(child);

	return 0;
}

int LuaScript::getParent(lua_State * lua)
{
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";

	GameObject * gameObject = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	GameObject * parent = gameObject->getParent();

	lua_pushlightuserdata(lua, parent);

	return 1;
}

int LuaScript::remove(lua_State * lua)
{
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";

	GameObject * parent = (GameObject*)lua_touserdata(lua, -2);
	GameObject * child = (GameObject*)lua_touserdata(lua, -1);

	parent->RemoveChild(child);

	return 0;
}

int LuaScript::getChildCount(lua_State * lua)
{
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";

	GameObject * gameObject = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	int count = gameObject->GetChildCount();

	lua_pushnumber(lua, count);

	return 1;
}

int LuaScript::getChildAt(lua_State * lua)
{
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";

	GameObject * gameObject = (GameObject*)lua_touserdata(lua, -2);
	int index = lua_tonumber(lua, -2);
	GameObject * child = gameObject->GetChildAt(index);

	lua_pushlightuserdata(lua, child);

	return 1;
}

int LuaScript::setLocalPosition(lua_State * lua)
{
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";

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
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));

	glm::vec3 pos = gameObj->getWorldPosition();

	lua_pushnumber(lua, pos.x);
	lua_pushnumber(lua, pos.y);
	lua_pushnumber(lua, pos.z);

	return 3;
}

int LuaScript::getLocalPos(lua_State * lua)
{
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));

	glm::vec3 pos = gameObj->getLocalPosition();

	lua_pushnumber(lua, pos.x);
	lua_pushnumber(lua, pos.y);
	lua_pushnumber(lua, pos.z);

	return 3;
}

int LuaScript::rotate(lua_State * lua)
{
	if (!lua_islightuserdata(lua, -5)) throw "Expect: game object";
	if (!lua_isnumber(lua, -4)) throw "Expect: number";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -5);
	float angle = lua_tonumber(lua, -4);
	glm::vec3 axis = glm::vec3(0, 0, 0);
	axis.x = lua_tonumber(lua, -3);
	axis.y = lua_tonumber(lua, -2);
	axis.z = lua_tonumber(lua, -1);

	gameObj->rotate(angle, axis);

	return 0;
}

int LuaScript::scale(lua_State * lua)
{
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";

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
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";

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
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: mesh";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -2);
	Mesh * mesh = (Mesh*)lua_touserdata(lua, -1);

	gameObj->setMesh(mesh);

	return 0;
}

int LuaScript::getMesh(lua_State * lua)
{
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	Mesh * mesh = gameObj->getMesh();

	lua_pushlightuserdata(lua, mesh);

	return 1;
}

int LuaScript::setMaterial(lua_State * lua)
{
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: material";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -2);
	AbstractMaterial * material = (AbstractMaterial*)lua_touserdata(lua, -1);

	gameObj->setMaterial(material);

	return 0;
}

int LuaScript::getMaterial(lua_State * lua)
{
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	AbstractMaterial * material = gameObj->getMaterial();

	lua_pushlightuserdata(lua, material);

	return 1;
}

int LuaScript::setName(lua_State * lua)
{
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isstring(lua, -1)) throw "Expect: string";

	string name = lua_tostring(lua, -1);
	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -2);

	gameObj->setName(name);

	return 0;
}

int LuaScript::getName(lua_State * lua)
{
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	const char * name = gameObj->getName().c_str();

	lua_pushstring(lua, name);

	return 1;
}

int LuaScript::getTime(lua_State * lua)
{
	float time = clock() / 500.0f;
	lua_pushnumber(lua, time);

	return 1;
}

int LuaScript::keyPressed(lua_State * lua)
{
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";

	int keyIndex = lua_tonumber(lua, lua_gettop(lua));
	bool pressed = false;

	if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(keyIndex)))
		pressed = true;

	lua_pushboolean(lua, pressed);

	return 1;
}

int LuaScript::keysBehaviour(lua_State * lua)
{
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";

	float moveSpeed = lua_tonumber(lua, -2);
	float turnSpeed = lua_tonumber(lua, -1);

	KeysBehaviour * keyBeh = new KeysBehaviour(moveSpeed, turnSpeed);
	lua_pushlightuserdata(lua, keyBeh);

	return 1;
}

int LuaScript::orbitBehaviour(lua_State * lua)
{/*
	if (!lua_islightuserdata(lua, -5)) throw "Expect: game object";
	if (!lua_isnumber(lua, -4)) throw "Expect: number";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -5);
	float radius = lua_tonumber(lua, -4);
	float rotationLimitX = lua_tonumber(lua, -3);
	float rotationLimitY = lua_tonumber(lua, -2);
	float rotationSpeed = lua_tonumber(lua, -1);

	OrbitBehaviour * orbitBeh = new OrbitBehaviour(gameObj, radius, rotationLimitX, rotationLimitY, rotationSpeed);
	lua_pushlightuserdata(lua, orbitBeh);
*/
	return 1;
}

int LuaScript::lookAtBehaviour(lua_State * lua)
{
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));

	LookAt * lookAt = new LookAt(gameObj);
	lua_pushlightuserdata(lua, lookAt);

	return 1;
}

int LuaScript::setBehaviour(lua_State * lua)
{
    /*
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: behaviour";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, -2);
	AbstractBehaviour * behaviour = (AbstractBehaviour*)lua_touserdata(lua, -1);

	gameObj->setBehaviour(behaviour);
*/
	return 0;

}

int LuaScript::getBehaviour(lua_State * lua)
{
    /*
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";

	GameObject * gameObj = (GameObject*)lua_touserdata(lua, lua_gettop(lua));
	AbstractBehaviour * behaviour = gameObj->getBehaviour();

	lua_pushlightuserdata(lua, behaviour);
*/
	return 1;
}

int LuaScript::distance(lua_State * lua)
{
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";

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
