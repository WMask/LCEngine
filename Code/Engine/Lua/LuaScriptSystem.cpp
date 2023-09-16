/**
* LuaScriptSystem.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
// put headers and lua546.lib compiled static library into Code/Engine/Lua/src folder
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
	if (luaState)
	{
		lua_close(luaState);
		luaState = nullptr;
	}
}

void LcLuaScriptSystem::RunScript(const std::string& script)
{
	if (!luaState) throw std::exception("LcLuaScriptSystem::RunScript(): Invalid Lua state");

	LcAny result;
	if (luaL_loadbuffer(luaState, script.c_str(), script.length(), "Initializer") == 0)
	{
		if (lua_pcall(luaState, 0, 0, 0) != 0) throw std::exception("LcLuaScriptSystem::RunScript(): Script error");
	}
	else
		throw std::exception("LcLuaScriptSystem::RunScript(): Buffer loading failed");
}

LcAny LcLuaScriptSystem::RunScriptEx(const std::string& script)
{
	if (!luaState) throw std::exception("LcLuaScriptSystem::RunScriptEx(): Invalid Lua state");

	LcAny result;
	if (luaL_loadbuffer(luaState, script.c_str(), script.length(), "Initializer") == 0)
	{
		if (lua_pcall(luaState, 0, 1, 0) != 0) throw std::exception("LcLuaScriptSystem::RunScriptEx(): Script error");

		auto top = lua_gettop(luaState);
		switch (lua_type(luaState, top))
		{
		case LUA_TNUMBER:
		case LUA_TBOOLEAN:
		case LUA_TSTRING:
			{
				int valid = 0;
				auto number = lua_tonumberx(luaState, top, &valid);
				if (valid)
				{
					result.fValue = (float)number;
					result.iValue = (int)number;
					result.bValue = (result.iValue != 0);
				}
				else
				{
					result.sValue = lua_tostring(luaState, top);
				}
			}
			break;
		}
	}
	else
		throw std::exception("LcLuaScriptSystem::RunScriptEx(): Buffer loading failed");

	return result;
}

TScriptSystemPtr GetScriptSystem()
{
	return std::make_shared<LcLuaScriptSystem>();
}
