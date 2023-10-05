/**
* LuaModulePhysics.cpp
* 05.10.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
#include "Core/LCUtils.h"
#include "Core/Physics.h"

#include "src/lua.hpp"


static int AddStaticBox(lua_State* luaState)
{
	LcSizef size;
	LcVector2 pos;
	int top = lua_gettop(luaState);

	if (!lua_istable(luaState, top - 1) ||
		!lua_istable(luaState, top - 0))
	{
		throw std::exception("AddStaticBox(): Invalid params");
	}
	else
	{
		pos = GetVector2(luaState, top - 1);
		size = GetVector2(luaState, top - 0);
	}

	auto app = GetApp(luaState);
	auto physics = app->GetContext().physics;
	if (!physics) throw std::exception("AddStaticBox(): Invalid Physics world");

	physics->AddStaticBox(pos, size);

	return 0;
}

static int AddDynamic(lua_State* luaState)
{
	LcVector2 pos;
	float radius;
	float density;
	bool fixedRotation;
	int top = lua_gettop(luaState);

	if (!lua_istable(luaState, top - 3))
	{
		throw std::exception("AddDynamic(): Invalid params");
	}
	else
	{
		pos = GetVector2(luaState, top - 3);
		radius = lua_tofloat(luaState, top - 2);
		density = lua_tofloat(luaState, top - 1);
		fixedRotation = lua_toboolean(luaState, top - 0) != 0;
	}

	auto app = GetApp(luaState);
	auto physics = app->GetContext().physics;
	if (!physics) throw std::exception("AddStaticBox(): Invalid Physics world");

	IPhysicsBody* body = physics->AddDynamic(pos, radius, density, fixedRotation);
	lua_pushlightuserdata(luaState, body);

	return 1;
}

static int AddDynamicBox(lua_State* luaState)
{
	LcVector2 pos;
	LcSizef size;
	float density;
	bool fixedRotation;
	int top = lua_gettop(luaState);

	if (!lua_istable(luaState, top - 3) ||
		!lua_istable(luaState, top - 2))
	{
		throw std::exception("AddDynamicBox(): Invalid params");
	}
	else
	{
		pos = GetVector2(luaState, top - 3);
		size = GetVector2(luaState, top - 2);
		density = lua_tofloat(luaState, top - 1);
		fixedRotation = lua_toboolean(luaState, top - 0) != 0;
	}

	auto app = GetApp(luaState);
	auto physics = app->GetContext().physics;
	if (!physics) throw std::exception("AddDynamicBox(): Invalid Physics world");

	IPhysicsBody* body = physics->AddDynamicBox(pos, size, density, fixedRotation);
	lua_pushlightuserdata(luaState, body);

	return 1;
}

static int SetUserData(lua_State* luaState)
{
	IPhysicsBody* body = nullptr;
	void* userData = nullptr;
	int top = lua_gettop(luaState);

	if (!lua_islightuserdata(luaState, top - 1) ||
		!lua_islightuserdata(luaState, top - 0))
	{
		throw std::exception("SetUserData(): Invalid params");
	}
	else
	{
		IPhysicsBody* body = static_cast<IPhysicsBody*>(lua_touserdata(luaState, top - 1));
		void* userData = lua_touserdata(luaState, top - 0);

		body->SetUserData(userData);
	}

	return 0;
}


void AddLuaModulePhysics(const LcAppContext& context, IScriptSystem* scriptSystem)
{
	auto luaSystem = static_cast<LcLuaScriptSystem*>(context.scripts);
	auto luaSystemCustom = static_cast<LcLuaScriptSystem*>(scriptSystem);
	auto luaState = luaSystem ? (luaSystemCustom ? luaSystemCustom->GetState() : luaSystem->GetState()) : nullptr;
	if (!luaState) throw std::exception("AddLuaModuleAudio(): Invalid Lua state");

	lua_pushcfunction(luaState, AddStaticBox);
	lua_setglobal(luaState, "AddStaticBox");

	lua_pushcfunction(luaState, AddDynamic);
	lua_setglobal(luaState, "AddDynamic");

	lua_pushcfunction(luaState, AddDynamicBox);
	lua_setglobal(luaState, "AddDynamicBox");

	lua_pushcfunction(luaState, SetUserData);
	lua_setglobal(luaState, "SetUserData");
}
