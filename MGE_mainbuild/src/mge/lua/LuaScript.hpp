#pragma once
extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}
#include <iostream>
#include <string>
#include <mge/behaviours/Component.hpp>

class World;
class GameObject;

class LuaScript : public Component
{
public:
	LuaScript(const char * path, World * world);
	void Update() override;
	void InvokeCollisionCallback(GameObject* other);
	void setOwner(GameObject* pOwner) override;
protected:
    virtual ~LuaScript();
private:
	lua_State * L;

	//core rendering and gameobject
	static int loadMesh(lua_State * lua);
	static int loadTexture(lua_State * lua);
	static int colorMaterial(lua_State * lua);
	static int textureMaterial(lua_State * lua);
	static int gameObject(lua_State * lua);
	static int camera(lua_State * lua);
	static int addToWorld(lua_State * lua);
	static int setMainCamera(lua_State * lua);
	static int setParent(lua_State * lua);
	static int getParent(lua_State * lua);
	static int getChildCount(lua_State * lua);
	static int getChildAt(lua_State * lua);
	static int setLocalPosition(lua_State * lua);
	static int getWorldPos(lua_State * lua);
	static int getLocalPos(lua_State * lua);
	static int rotate(lua_State * lua);
	static int scale(lua_State * lua);
	static int translate(lua_State * lua);
	static int setMesh(lua_State * lua);
	static int getMesh(lua_State * lua);
	static int setMaterial(lua_State * lua);
	static int getMaterial(lua_State * lua);
	static int setName(lua_State * lua);
	static int getName(lua_State * lua);

	//utils
	static int getTime(lua_State * lua);
	static int distance(lua_State * lua);
	static int randomRange(lua_State * lua);
	static int randomRoll(lua_State * lua);

	//input
	static int keyPressed(lua_State * lua);
	static int keyUp(lua_State * lua);
	static int keyDown(lua_State * lua);
	static int mouseButton(lua_State * lua);
	static int mouseButtonUp(lua_State * lua);
	static int mousePosition(lua_State * lua);

	//components
	static int attachComponent(lua_State * lua);
	static int sphereCollider(lua_State * lua);
	static int boxCollider(lua_State * lua);
	static int wallCollider(lua_State * lua);
	static int luaScript(lua_State * lua);

	//levels/scenes
	static int loadLevel(lua_State * lua);



};
