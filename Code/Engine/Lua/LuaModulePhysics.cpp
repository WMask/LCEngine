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

static int SetBodyPos(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top - 1) ||
		!lua_istable(luaState, top - 0))
	{
		throw std::exception("SetBodyPos(): Invalid params");
	}
	else
	{
		IPhysicsBody* body = static_cast<IPhysicsBody*>(lua_touserdata(luaState, top - 1));
		LcVector2 pos = GetVector2(luaState, top - 0);

		body->SetPos(pos);
	}

	return 0;
}

static int ApplyBodyImpulse(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top - 1) ||
		!lua_istable(luaState, top - 0))
	{
		throw std::exception("ApplyBodyImpulse(): Invalid params");
	}
	else
	{
		IPhysicsBody* body = static_cast<IPhysicsBody*>(lua_touserdata(luaState, top - 1));
		LcVector2 impulse = GetVector2(luaState, top - 0);

		body->ApplyImpulse(impulse);
	}

	return 0;
}

static int GetBodyPos(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top))
	{
		throw std::exception("GetBodyPos(): Invalid params");
	}
	else
	{
		IPhysicsBody* body = static_cast<IPhysicsBody*>(lua_touserdata(luaState, top));
		LcVector2 pos = body->GetPos();

		lua_createtable(luaState, 0, 2);
		lua_pushnumber(luaState, pos.x);
		lua_setfield(luaState, -2, "x");
		lua_pushnumber(luaState, pos.y);
		lua_setfield(luaState, -2, "y");
	}

	return 1;
}

static int SetBodyVelocity(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top - 1) ||
		!lua_istable(luaState, top - 0))
	{
		throw std::exception("SetBodyVelocity(): Invalid params");
	}
	else
	{
		IPhysicsBody* body = static_cast<IPhysicsBody*>(lua_touserdata(luaState, top - 1));
		LcVector2 vel = GetVector2(luaState, top - 0);

		body->SetVelocity(vel);
	}

	return 0;
}

static int GetBodyVelocity(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top))
	{
		throw std::exception("GetBodyVelocity(): Invalid params");
	}
	else
	{
		IPhysicsBody* body = static_cast<IPhysicsBody*>(lua_touserdata(luaState, top));
		LcVector2 vel = body->GetVelocity();

		lua_createtable(luaState, 0, 2);
		lua_pushnumber(luaState, vel.x);
		lua_setfield(luaState, -2, "x");
		lua_pushnumber(luaState, vel.y);
		lua_setfield(luaState, -2, "y");
	}

	return 1;
}

static int IsBodyFalling(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isuserdata(luaState, top))
	{
		throw std::exception("IsBodyFalling(): Invalid params");
	}
	else
	{
		IPhysicsBody* body = static_cast<IPhysicsBody*>(lua_touserdata(luaState, top));

		lua_pushboolean(luaState, body->IsFalling() ? 1 : 0);
	}

	return 1;
}

static int SetBodyUserData(lua_State* luaState)
{
	IPhysicsBody* body = nullptr;
	void* userData = nullptr;
	int top = lua_gettop(luaState);

	if (!lua_islightuserdata(luaState, top - 1) ||
		!lua_islightuserdata(luaState, top - 0))
	{
		throw std::exception("SetBodyUserData(): Invalid params");
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
	auto luaState = luaSystemCustom ? luaSystemCustom->GetState() : luaSystem->GetState();
	if (!luaState) throw std::exception("AddLuaModulePhysics(): Invalid Lua state");

	lua_pushcfunction(luaState, AddStaticBox);
	lua_setglobal(luaState, "AddStaticBox");

	lua_pushcfunction(luaState, AddDynamic);
	lua_setglobal(luaState, "AddDynamic");

	lua_pushcfunction(luaState, AddDynamicBox);
	lua_setglobal(luaState, "AddDynamicBox");

	lua_pushcfunction(luaState, ApplyBodyImpulse);
	lua_setglobal(luaState, "ApplyBodyImpulse");

	lua_pushcfunction(luaState, SetBodyPos);
	lua_setglobal(luaState, "SetBodyPos");

	lua_pushcfunction(luaState, GetBodyPos);
	lua_setglobal(luaState, "GetBodyPos");

	lua_pushcfunction(luaState, SetBodyVelocity);
	lua_setglobal(luaState, "SetBodyVelocity");

	lua_pushcfunction(luaState, GetBodyVelocity);
	lua_setglobal(luaState, "GetBodyVelocity");

	lua_pushcfunction(luaState, IsBodyFalling);
	lua_setglobal(luaState, "IsBodyFalling");

	lua_pushcfunction(luaState, SetBodyUserData);
	lua_setglobal(luaState, "SetBodyUserData");
}
