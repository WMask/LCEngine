/**
* LuaScriptSystem.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"

// put headers and lua546.lib compiled static library into Code/Engine/Lua/src folder
#include "src/lua.hpp"

static const char* CurPath = nullptr;


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
				luaopen_debug(luaState);
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

	LC_TRY

	LcAny result;
	if (luaL_loadbuffer(luaState, script.c_str(), script.length(), "Initializer") == 0)
	{
		if (lua_pcall(luaState, 0, 0, 0) != 0)
		{
			int top = lua_gettop(luaState);
			auto error = lua_tostring(luaState, top);
			throw std::exception(error);
		}
	}
	else
	{
		int top = lua_gettop(luaState);
		auto error = lua_tostring(luaState, top);
		throw std::exception(error);
	}

	LC_CATCH{ LC_THROW_EX("LcLuaScriptSystem():'", CurPath, "'"); }
}

void LcLuaScriptSystem::RunScriptFile(const char* filePath)
{
	std::string fileText = ReadTextFile(filePath);
	CurPath = filePath;
	RunScript(fileText);
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

LcAny LcLuaScriptSystem::RunScriptFileEx(const char* filePath)
{
	std::string fileText = ReadTextFile(filePath);
	CurPath = filePath;
	return RunScriptEx(fileText);
}


IApplication* GetApp(lua_State* luaState)
{
	lua_getglobal(luaState, LuaAppGlobalName);
	auto appPtr = lua_touserdata(luaState, -1);
	auto app = static_cast<IApplication*>(appPtr);
	if (!app) throw std::exception("GetApp(): Invalid Application");
	return app;
}

IWorld* GetWorld(lua_State* luaState)
{
	lua_getglobal(luaState, LuaWorldGlobalName);
	auto worldPtr = lua_touserdata(luaState, -1);
	auto world = static_cast<IWorld*>(worldPtr);
	if (!world) throw std::exception("GetWorld(): Invalid World");
	return world;
}

LcColor4 GetColor(struct lua_State* luaState, int table)
{
	if (!lua_istable(luaState, table)) throw std::exception("GetColor(): Invalid table");

	LcColor4 color{};

	lua_getfield(luaState, table, "r");
	if (!lua_isnumber(luaState, -1)) throw std::exception("GetColor(): Invalid table");
	color.x = lua_tofloat(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "g");
	if (!lua_isnumber(luaState, -1)) throw std::exception("GetColor(): Invalid table");
	color.y = lua_tofloat(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "b");
	if (!lua_isnumber(luaState, -1)) throw std::exception("GetColor(): Invalid table");
	color.z = lua_tofloat(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "a");
	color.w = lua_isnumber(luaState, -1) ? lua_tofloat(luaState, -1) : 1.0f;
	lua_pop(luaState, 1);

	return color;
}

LcVector2 GetVector2(struct lua_State* luaState, int table)
{
	if (!lua_istable(luaState, table)) throw std::exception("GetVector2(): Invalid table");

	LcVector2 vector{};

	lua_getfield(luaState, table, "x");
	if (!lua_isnumber(luaState, -1)) throw std::exception("GetVector2(): Invalid table");
	vector.x = lua_tofloat(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "y");
	if (!lua_isnumber(luaState, -1)) throw std::exception("GetVector2(): Invalid table");
	vector.y = lua_tofloat(luaState, -1);
	lua_pop(luaState, 1);

	return vector;
}

LcVector3 GetVector(struct lua_State* luaState, int table)
{
	if (!lua_istable(luaState, table)) throw std::exception("GetVector(): Invalid table");

	LcVector3 vector{};

	lua_getfield(luaState, table, "x");
	if (!lua_isnumber(luaState, -1)) throw std::exception("GetVector(): Invalid table");
	vector.x = lua_tofloat(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "y");
	if (!lua_isnumber(luaState, -1)) throw std::exception("GetVector(): Invalid table");
	vector.y = lua_tofloat(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "z");
	vector.z = lua_isnumber(luaState, -1) ? lua_tofloat(luaState, -1) : 0.0f;
	lua_pop(luaState, 1);

	return vector;
}

LcBasicParticleSettings GetParticleSettings(struct lua_State* luaState, int table)
{
	if (!lua_istable(luaState, table)) throw std::exception("AddParticlesComponent(): Invalid table");

	LcBasicParticleSettings settings;

	lua_getfield(luaState, table, "frameSize");
	if (!lua_istable(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");

	{
		lua_getfield(luaState, -1, "x");
		if (!lua_isnumber(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
		settings.frameSize.x = lua_tofloat(luaState, -1);
		lua_pop(luaState, 1);

		lua_getfield(luaState, -1, "y");
		if (!lua_isnumber(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
		settings.frameSize.y = lua_tofloat(luaState, -1);
		lua_pop(luaState, 1);
	}

	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "numFrames");
	if (!lua_isnumber(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
	settings.numFrames = lua_toint(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "movementRadius");
	if (!lua_isnumber(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
	settings.movementRadius = lua_tofloat(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "lifetime");
	settings.lifetime = lua_isnumber(luaState, -1) ? lua_tofloat(luaState, -1) : -1.0f;
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "fadeInRate");
	settings.fadeInRate = lua_isnumber(luaState, -1) ? lua_tofloat(luaState, -1) : -1.0f;
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "fadeOutRate");
	settings.fadeOutRate = lua_isnumber(luaState, -1) ? lua_tofloat(luaState, -1) : -1.0f;
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "speed");
	settings.speed = lua_isnumber(luaState, -1) ? lua_tofloat(luaState, -1) : 1.0f;
	lua_pop(luaState, 1);

	return settings;
}


TScriptSystemPtr GetScriptSystem()
{
	return std::make_shared<LcLuaScriptSystem>();
}
