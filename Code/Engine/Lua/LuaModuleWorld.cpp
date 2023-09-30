/**
* LuaModuleWorld.cpp
* 30.09.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
#include "Core/Visual.h"

#include "src/lua.hpp"


static int AddSprite(lua_State* luaState)
{
	auto world = GetWorld(luaState);

	auto x = lua_tofloat(luaState, -5);
	auto y = lua_tofloat(luaState, -4);
	auto width = lua_tofloat(luaState, -3);
	auto height = lua_tofloat(luaState, -2);

	auto sprite = world->AddSprite(x, y, width, height);
	lua_pushlightuserdata(luaState, sprite);

	return 1;
}

static int AddTintComponent(lua_State* luaState)
{
	auto world = GetWorld(luaState);
	if (!world) throw std::exception("AddTintComponent(): Invalid World");

	IVisual* visual = nullptr;
	float r, g, b;

	if (lua_isuserdata(luaState, -5))
	{
		visual = static_cast<IVisual*>(lua_touserdata(luaState, -5));
		r = lua_tofloat(luaState, -4);
		g = lua_tofloat(luaState, -3);
		b = lua_tofloat(luaState, -2);
	}
	else
	{
		r = lua_tofloat(luaState, -4);
		g = lua_tofloat(luaState, -3);
		b = lua_tofloat(luaState, -2);
	}

	if (visual)
	{
		if (auto app = GetApp(luaState))
		{
			visual->AddTintComponent(LcColor3(r, g, b), app->GetContext());
		}
		else throw std::exception("AddTintComponent(): Invalid Application");
	}
	else
	{
		world->GetVisualHelper().AddTintComponent(LcColor3(r, g, b));
	}

	return 0;
}

void AddLuaModuleWorld(const LcAppContext& context, IScriptSystem* scriptSystem)
{
	auto luaSystem = static_cast<LcLuaScriptSystem*>(context.scripts);
	auto luaSystemCustom = static_cast<LcLuaScriptSystem*>(scriptSystem);
	auto luaState = luaSystem ? (luaSystemCustom ? luaSystemCustom->GetState() : luaSystem->GetState()) : nullptr;
	if (!luaState) throw std::exception("AddLuaModuleWorld(): Invalid Lua state");

	lua_pushlightuserdata(luaState, context.world);
	lua_setglobal(luaState, LuaWorldGlobalName);

	lua_pushcfunction(luaState, AddSprite);
	lua_setglobal(luaState, "AddSprite");

	lua_pushcfunction(luaState, AddTintComponent);
	lua_setglobal(luaState, "AddTintComponent");
}
