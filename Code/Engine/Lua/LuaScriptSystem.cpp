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

static int SetScriptHandlerName(lua_State* luaState);
static int SetTag(lua_State* luaState);
static int GetTag(lua_State* luaState);
static int AddToRoot(lua_State* luaState);
static int RemoveFromRoot(lua_State* luaState);
static int IsRooted(lua_State* luaState);


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

	lua_pushlightuserdata(luaState, this);
	lua_setglobal(luaState, LuaScriptGlobalName);

	lua_pushlightuserdata(luaState, context.app);
	lua_setglobal(luaState, LuaAppGlobalName);

	lua_pushlightuserdata(luaState, context.world);
	lua_setglobal(luaState, LuaWorldGlobalName);

	lua_pushlightuserdata(luaState, context.physics);
	lua_setglobal(luaState, LuaPhysicsGlobalName);

	lua_pushlightuserdata(luaState, context.audio);
	lua_setglobal(luaState, LuaAudioGlobalName);

	lua_pushlightuserdata(luaState, context.input);
	lua_setglobal(luaState, LuaInputGlobalName);

	lua_pushcfunction(luaState, SetScriptHandlerName);
	lua_setglobal(luaState, "SetScriptHandlerName");

	lua_pushcfunction(luaState, SetTag);
	lua_setglobal(luaState, "SetTag");

	lua_pushcfunction(luaState, GetTag);
	lua_setglobal(luaState, "GetTag");

	lua_pushcfunction(luaState, AddToRoot);
	lua_setglobal(luaState, "AddToRoot");

	lua_pushcfunction(luaState, RemoveFromRoot);
	lua_setglobal(luaState, "RemoveFromRoot");

	lua_pushcfunction(luaState, IsRooted);
	lua_setglobal(luaState, "IsRooted");
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

int SetTag(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top - 1) ||
		!lua_isinteger(luaState, top - 0))
	{
		throw std::exception("SetTag(): Invalid object");
	}
	else
	{
		IObjectBase* object = static_cast<IObjectBase*>(lua_touserdata(luaState, top - 1));
		int tag = lua_toint(luaState, top - 0);
	}

	return 0;
}

int GetTag(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top))
	{
		throw std::exception("GetTag(): Invalid object");
	}
	else
	{
		IObjectBase* object = static_cast<IObjectBase*>(lua_touserdata(luaState, top));
		lua_pushinteger(luaState, object->GetTag());
	}

	return 1;
}

int AddToRoot(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top))
	{
		throw std::exception("AddToRoot(): Invalid object");
	}
	else
	{
		IObjectBase* object = static_cast<IObjectBase*>(lua_touserdata(luaState, top));

		object->AddToRoot();
	}

	return 0;
}

int RemoveFromRoot(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top))
	{
		throw std::exception("RemoveFromRoot(): Invalid object");
	}
	else
	{
		IObjectBase* object = static_cast<IObjectBase*>(lua_touserdata(luaState, top));

		object->RemoveFromRoot();
	}

	return 0;
}

int IsRooted(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top))
	{
		throw std::exception("IsRooted(): Invalid object");
	}
	else
	{
		IObjectBase* object = static_cast<IObjectBase*>(lua_touserdata(luaState, top));
		lua_pushboolean(luaState, object->IsRooted() ? 1 : 0);
	}

	return 1;
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

IPhysicsWorld* GetPhysWorld(struct lua_State* luaState)
{
	lua_getglobal(luaState, LuaPhysicsGlobalName);
	auto worldPtr = lua_touserdata(luaState, -1);
	auto world = static_cast<IPhysicsWorld*>(worldPtr);
	if (!world) throw std::exception("GetPhysWorld(): Invalid World");
	return world;
}

IAudioSystem* GetAudio(struct lua_State* luaState)
{
	lua_getglobal(luaState, LuaAudioGlobalName);
	auto audioPtr = lua_touserdata(luaState, -1);
	auto audio = static_cast<IAudioSystem*>(audioPtr);
	if (!audio) throw std::exception("GetAudio(): Invalid Audio system");
	return audio;
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

void PushAny(struct lua_State* luaState, const LcAny& any)
{
	switch (any.type)
	{
	case LcAny::LcAnyType::StringAny: lua_pushstring(luaState, any.sValue.c_str()); break;
	case LcAny::LcAnyType::FloatAny: lua_pushnumber(luaState, any.fValue); break;
	case LcAny::LcAnyType::IntAny: lua_pushinteger(luaState, any.iValue); break;
	case LcAny::LcAnyType::BoolAny: lua_pushboolean(luaState, any.bValue ? 1 : 0); break;
	}
}


TScriptSystemPtr GetScriptSystem()
{
	return std::make_shared<LcLuaScriptSystem>();
}
