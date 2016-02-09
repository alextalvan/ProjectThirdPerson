#ifndef LUAENGINE_H
#define LUAENGINE_H

#include <string>
#include <iostream>
#include "lua.hpp"

class World;

class LuaEngine
{
public:
    //Constructor
    LuaEngine(World * world) : m_L(luaL_newstate()), _world(world) { luaL_openlibs(m_L); }
    LuaEngine(const LuaEngine& other);  //non-construction copy
    LuaEngine& operator=(const LuaEngine&); //non-copy
    ~LuaEngine(void) { lua_close(m_L); } //Destructor clean-up

    //returns lua state object
    lua_State* L();
    //Execute Lua File
    void ExecuteFile(std::string file);

    //Execute Lua Expression (contained in string)
    void ExecuteString(const char* expression);

private:
    //member lua state
    lua_State* m_L;
    World * _world;

private:

    //handle errors
    void report_errors(int state);
    void SetWorld();
};

#endif // LUAENGINE_H
