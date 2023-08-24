/**
* LuaScriptSystem.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"

#include <stdio.h>
#include <string.h>
#include "src/lua.hpp"


LcLuaScriptSystem::LcLuaScriptSystem(bool openBaseDefaultLibs, bool openAllDefaultLibs)
{
	luaState = luaL_newstate();
	if (luaState)
	{
		if (openAllDefaultLibs)
			luaL_openlibs(luaState);
		else
		{
			if (openBaseDefaultLibs)
			{
				luaopen_base(luaState);
				luaopen_table(luaState);
				luaopen_string(luaState);
				luaopen_math(luaState);
			}
		}
	}
	else
		throw std::exception("LcLuaScriptSystem(): Cannot create Lua state");
}

LcLuaScriptSystem::~LcLuaScriptSystem()
{
	if (luaState) lua_close(luaState);
}

void LcLuaScriptSystem::RunScript(const std::string& script)
{
	if (!luaState) throw std::exception("LcLuaScriptSystem::RunScript(): Invalid Lua state");

	if (luaL_loadbuffer(luaState, script.c_str(), script.length(), "Initializer") == 0)
	{
		lua_pcall(luaState, 0, 0, 0);
	}
	else
		throw std::exception("LcLuaScriptSystem::RunScript(): Buffer loading failed");
}
