/**
* LuaModuleApplication.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
#include "Core/Visual.h"

#include "src/lua.hpp"


static int RequestQuit(lua_State* luaState)
{
	auto app = GetApp(luaState);
	app->RequestQuit();
	return 0;
}

void AddLuaModuleApplication(const LcAppContext& context, IScriptSystem* scriptSystem)
{
	auto luaSystem = static_cast<LcLuaScriptSystem*>(context.scripts);
	auto luaSystemCustom = static_cast<LcLuaScriptSystem*>(scriptSystem);
	auto luaState = luaSystemCustom ? luaSystemCustom->GetState() : luaSystem->GetState();
	if (!luaState) throw std::exception("AddLuaModuleApplication(): Invalid Lua state");

	lua_pushcfunction(luaState, RequestQuit);
	lua_setglobal(luaState, "RequestQuit");
}
