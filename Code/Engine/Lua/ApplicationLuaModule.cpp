/**
* ApplicationLuaModule.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "Lua/ApplicationLuaModule.h"
#include "Lua/LuaScriptSystem.h"
#include "Application/Application.h"
#include "src/lua.hpp"


LcApplicationLuaModule::LcApplicationLuaModule(IApplication& appRef) : app(appRef)
{
}

LcApplicationLuaModule::~LcApplicationLuaModule()
{
}

static int requestQuit(lua_State* luaState)
{
	lua_getglobal(luaState, "app");
	int top = lua_gettop(luaState);
	auto appPtr = lua_touserdata(luaState, top);
	if (auto app = (IApplication*)appPtr)
	{
		app->RequestQuit();
	}

	return 0;
}

void LcApplicationLuaModule::AddTo(IScriptSystem& system)
{
	auto& luaSystem = (LcLuaScriptSystem&)system;
	if (auto luaState = luaSystem.GetState())
	{
		lua_pushlightuserdata(luaState, &app);
		lua_setglobal(luaState, "app");

		lua_pushcfunction(luaState, requestQuit);
		lua_setglobal(luaState, "requestQuit");
	}
	else
		throw std::exception("LcApplicationLuaModule::Add(): Invalid Lua state");
}

IScriptModule& GetApplicationLuaModule(IApplication& appRef)
{
	static LcApplicationLuaModule instance(appRef);
	return instance;
}
