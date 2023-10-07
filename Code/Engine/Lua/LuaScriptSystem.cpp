/**
* LuaScriptSystem.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
#include "World/WorldInterface.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"

// put headers and lua546.lib compiled static library into Code/Engine/Lua/src folder
#include "src/lua.hpp"

static const char* CurPath = nullptr;

static int SetTag(lua_State* luaState);
static int GetTag(lua_State* luaState);
static int SetScriptHandlerName(lua_State* luaState);


LcLuaScriptSystem::~LcLuaScriptSystem()
{
	if (luaState)
	{
		lua_close(luaState);
		luaState = nullptr;
	}
}

void LcLuaScriptSystem::Init(const LcAppContext& context)
{
	luaState = luaL_newstate();
	if (!luaState)
	{
		throw std::exception("LcLuaScriptSystem::Init(): Cannot create Lua state");
	}

	if (openAllDefaultLibs)
	{
		luaL_openlibs(luaState);
	}
	else if (openBaseDefaultLibs)
	{
		luaL_requiref(luaState, "base", luaopen_base, 1);
		luaL_requiref(luaState, "table", luaopen_table, 1);
		luaL_requiref(luaState, "string", luaopen_string, 1);
		luaL_requiref(luaState, "math", luaopen_math, 1);
		luaL_requiref(luaState, "debug", luaopen_debug, 1);
	}

	lua_createtable(luaState, 0, 4);
	lua_pushinteger(luaState, (int)LcScriptHandler::Update);
	lua_setfield(luaState, -2, "Update");
	lua_pushinteger(luaState, (int)LcScriptHandler::Actions);
	lua_setfield(luaState, -2, "Actions");
	lua_pushinteger(luaState, (int)LcScriptHandler::Keys);
	lua_setfield(luaState, -2, "Keys");
	lua_pushinteger(luaState, (int)LcScriptHandler::Axis);
	lua_setfield(luaState, -2, "Axis");
	lua_setglobal(luaState, "ScriptHandler");

	lua_pushcfunction(luaState, SetScriptHandlerName);
	lua_setglobal(luaState, "SetScriptHandlerName");

	lua_pushlightuserdata(luaState, this);
	lua_setglobal(luaState, LuaScriptGlobalName);

	lua_pushlightuserdata(luaState, context.app);
	lua_setglobal(luaState, LuaAppGlobalName);

	lua_pushlightuserdata(luaState, context.world);
	lua_setglobal(luaState, LuaWorldGlobalName);

	lua_pushlightuserdata(luaState, context.input);
	lua_setglobal(luaState, LuaInputGlobalName);

	lua_pushcfunction(luaState, SetTag);
	lua_setglobal(luaState, "SetTag");

	lua_pushcfunction(luaState, GetTag);
	lua_setglobal(luaState, "GetTag");
}

void LcLuaScriptSystem::RunScript(const std::string& script)
{
	LC_TRY

	if (!luaState) throw std::exception("LcLuaScriptSystem::RunScript(): Invalid Lua state");

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

	LC_CATCH{ LC_THROW_EX("LcLuaScriptSystem::RunScript():'", CurPath, "'"); }
}

LcAny LcLuaScriptSystem::RunScriptEx(const std::string& script)
{
	LcAny result;

	LC_TRY

	if (!luaState) throw std::exception("LcLuaScriptSystem::RunScriptEx(): Invalid Lua state");

	if (luaL_loadbuffer(luaState, script.c_str(), script.length(), "Initializer") != 0)
	{
		throw std::exception("LcLuaScriptSystem::RunScriptEx(): Buffer loading failed");
	}

	if (lua_pcall(luaState, 0, 1, 0) != 0)
	{
		throw std::exception("LcLuaScriptSystem::RunScriptEx(): Script error");
	}

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

	LC_CATCH{ LC_THROW_EX("LcLuaScriptSystem::RunScriptEx():'", CurPath, "'"); }

	return result;
}

void LcLuaScriptSystem::RunScriptFile(const char* filePath)
{
	std::string fileText = ReadTextFile(filePath);
	CurPath = filePath;
	RunScript(fileText);
}

LcAny LcLuaScriptSystem::RunScriptFileEx(const char* filePath)
{
	std::string fileText = ReadTextFile(filePath);
	CurPath = filePath;
	return RunScriptEx(fileText);
}

void LcLuaScriptSystem::RunUpdateHandler(float deltaSeconds)
{
	lua_getglobal(luaState, updateHandlerName.c_str());
	lua_pushnumber(luaState, deltaSeconds);
	lua_call(luaState, 1, 0);
}

void LcLuaScriptSystem::RunActionsHandler(const std::string& action)
{
	lua_getglobal(luaState, actionsHandlerName.c_str());
	lua_pushstring(luaState, action.c_str());
	lua_call(luaState, 1, 0);
}

void LcLuaScriptSystem::RunKeysHandler(int key, LcKeyState keyEvent)
{
	lua_getglobal(luaState, keysHandlerName.c_str());
	lua_pushinteger(luaState, key);
	lua_pushinteger(luaState, (int)keyEvent);
	lua_call(luaState, 2, 0);
}

void LcLuaScriptSystem::RunAxisHandler(int axis, float x, float y)
{
	lua_getglobal(luaState, axisHandlerName.c_str());
	lua_pushinteger(luaState, axis);
	lua_pushnumber(luaState, x);
	lua_pushnumber(luaState, y);
	lua_call(luaState, 3, 0);
}

void LcLuaScriptSystem::SetHandlerName(LcScriptHandler type, const char* name)
{
	switch (type)
	{
	case LcScriptHandler::Update: updateHandlerName = name; break;
	case LcScriptHandler::Actions: actionsHandlerName = name; break;
	case LcScriptHandler::Keys: keysHandlerName = name; break;
	case LcScriptHandler::Axis: axisHandlerName = name; break;
	}
}

const char* LcLuaScriptSystem::GetHandlerName(LcScriptHandler type) const
{
	switch (type)
	{
	case LcScriptHandler::Update: return updateHandlerName.c_str();
	case LcScriptHandler::Actions: return actionsHandlerName.c_str();
	case LcScriptHandler::Keys: return keysHandlerName.c_str();
	case LcScriptHandler::Axis: return axisHandlerName.c_str();
	}

	return nullptr;
}


int SetScriptHandlerName(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isinteger(luaState, top - 1) ||
		!lua_isstring(luaState, top - 0))
	{
		throw std::exception("SetScriptHandlerName(): Invalid params");
	}
	else
	{
		LcScriptHandler type = static_cast<LcScriptHandler>(lua_toint(luaState, top - 1));
		const char* name = lua_tostring(luaState, top - 0);

		if (auto script = GetScript(luaState))
		{
			script->SetHandlerName(type, name);
		}
	}

	return 0;
}

IScriptSystem* GetScript(struct lua_State* luaState)
{
	lua_getglobal(luaState, LuaScriptGlobalName);
	auto scriptPtr = lua_touserdata(luaState, -1);
	auto script = static_cast<IScriptSystem*>(scriptPtr);
	if (!script) throw std::exception("GetScript(): Invalid Script system");
	return script;
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

IInputSystem* GetInput(struct lua_State* luaState)
{
	lua_getglobal(luaState, LuaInputGlobalName);
	auto inputPtr = lua_touserdata(luaState, -1);
	auto input = static_cast<IInputSystem*>(inputPtr);
	if (!input) throw std::exception("GetInput(): Invalid Input");
	return input;
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

int SetTag(lua_State* luaState)
{
	IVisual* visual = nullptr;
	int tag = -1;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top - 1))
	{
		visual = static_cast<IVisual*>(lua_touserdata(luaState, top - 1));
		tag = lua_toint(luaState, top - 0);
	}
	else
	{
		tag = lua_toint(luaState, top - 0);
	}

	if (visual)
	{
		auto app = GetApp(luaState);
		visual->SetTag(tag);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetVisualHelper().SetTag(tag);
	}

	return 0;
}

int GetTag(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top))
	{
		auto visual = static_cast<IVisual*>(lua_touserdata(luaState, top));
		lua_pushinteger(luaState, visual->GetTag());
	}
	else
	{
		throw std::exception("GetTag(): Invalid object");
	}

	return 1;
}


TScriptSystemPtr GetScriptSystem()
{
	return std::make_shared<LcLuaScriptSystem>();
}
