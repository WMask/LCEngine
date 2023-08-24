/**
* ApplicationLuaModule.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "Lua/ApplicationLuaModule.h"
#include "Lua/LuaScriptSystem.h"
#include "Application/Application.h"
#include "src/lua.hpp"


LcApplicationLuaModule::LcApplicationLuaModule(std::weak_ptr<IApplication> appPtr) : weakApp(appPtr)
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

void LcApplicationLuaModule::Add(IScriptSystem& system)
{
	auto app = weakApp.lock();
	if (!app) throw std::exception("LcApplicationLuaModule::Add(): Invalid Application pointer");

	auto& luaSystem = (LcLuaScriptSystem&)system;
	if (auto luaState = luaSystem.GetState())
	{
		lua_pushlightuserdata(luaState, app.get());
		lua_setglobal(luaState, "app");

		lua_pushcfunction(luaState, requestQuit);
		lua_setglobal(luaState, "requestQuit");
	}
	else
		throw std::exception("LcApplicationLuaModule::Add(): Invalid Lua state");
}

IScriptModule* GetApplicationLuaModule(std::weak_ptr<IApplication> appPtr)
{
	static LcApplicationLuaModule instance(appPtr);
	return &instance;
}
