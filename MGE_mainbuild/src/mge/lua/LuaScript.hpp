#pragma once
extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}
#include <iostream>
#include <vector>
#include <mge/behaviours/Component.hpp>

class World;
class GameObject;

class LuaScript : public Component
{
public:
	LuaScript(const char * path, World * world);
	void Update() override;
	void InvokeCollisionCallback(GameObject* other);
protected:
    virtual ~LuaScript();
private:
	lua_State * L;
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
	static int remove(lua_State * lua);
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
	static int getTime(lua_State * lua);
	static int keyPressed(lua_State * lua);
	static int keysBehaviour(lua_State * lua);
	static int orbitBehaviour(lua_State * lua);
	static int lookAtBehaviour(lua_State * lua);
	static int setBehaviour(lua_State * lua);
	static int getBehaviour(lua_State * lua);
	static int distance(lua_State * lua);
};
