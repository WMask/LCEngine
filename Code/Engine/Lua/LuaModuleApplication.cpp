/**
* LuaModuleApplication.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
#include "Application/ApplicationInterface.h"

#include "src/lua.hpp"


static IApplication* getApp(lua_State* luaState)
{
	lua_getglobal(luaState, "app");
	int top = lua_gettop(luaState);
	auto appPtr = lua_touserdata(luaState, top);
	return static_cast<IApplication*>(appPtr);
}

static int requestQuit(lua_State* luaState)
{
	if (auto app = getApp(luaState))
	{
		app->RequestQuit();
	}

	return 0;
}

void AddLuaModuleApplication(const LcAppContext& context)
{
	auto luaSystem = static_cast<LcLuaScriptSystem*>(context.scripts);
	auto luaState = luaSystem ? luaSystem->GetState() : nullptr;
	if (!luaState) throw std::exception("AddLuaModuleApplication(): Invalid Lua state");

	lua_pushlightuserdata(luaState, context.app);
	lua_setglobal(luaState, "app");

	lua_pushcfunction(luaState, requestQuit);
	lua_setglobal(luaState, "RequestQuit");
}
