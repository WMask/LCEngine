/**
* LuaModuleAudio.cpp
* 04.10.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
#include "Core/LCUtils.h"
#include "Core/InputSystem.h"

#include "src/lua.hpp"


static int IsKeyPressed(lua_State* luaState)
{
	int top = lua_gettop(luaState);

	if (!lua_isinteger(luaState, top))
	{
		throw std::exception("IsKeyPressed(): Invalid params");
	}
	else
	{
		int key = lua_toint(luaState, top);
		auto input = GetInput(luaState);

		lua_pushboolean(luaState, input->GetActiveInputDevice()->GetState()[key]);
	}

	return 1;
}


void AddLuaModuleInput(const LcAppContext& context, IScriptSystem* scriptSystem)
{
	auto luaSystem = static_cast<LcLuaScriptSystem*>(context.scripts);
	auto luaSystemCustom = static_cast<LcLuaScriptSystem*>(scriptSystem);
	auto luaState = luaSystemCustom ? luaSystemCustom->GetState() : luaSystem->GetState();
	if (!luaState) throw std::exception("AddLuaModuleInput(): Invalid Lua state");

	lua_pushcfunction(luaState, IsKeyPressed);
	lua_setglobal(luaState, "IsKeyPressed");

	lua_createtable(luaState, 0, 8);
	lua_pushnumber(luaState, 0);
	lua_setfield(luaState, -2, "Up");
	lua_pushnumber(luaState, 1);
	lua_setfield(luaState, -2, "Down");
	lua_setglobal(luaState, "KeyState");

	lua_createtable(luaState, 0, 8);
	lua_pushnumber(luaState, LcKeys::Escape);
	lua_setfield(luaState, -2, "Escape");
	lua_pushnumber(luaState, LcKeys::Space);
	lua_setfield(luaState, -2, "Space");
	lua_pushnumber(luaState, LcKeys::Return);
	lua_setfield(luaState, -2, "Return");
	lua_pushnumber(luaState, LcKeys::Enter);
	lua_setfield(luaState, -2, "Enter");
	lua_pushnumber(luaState, LcKeys::Up);
	lua_setfield(luaState, -2, "Up");
	lua_pushnumber(luaState, LcKeys::Right);
	lua_setfield(luaState, -2, "Right");
	lua_pushnumber(luaState, LcKeys::Down);
	lua_setfield(luaState, -2, "Down");
	lua_pushnumber(luaState, LcKeys::Left);
	lua_setfield(luaState, -2, "Left");
	lua_setglobal(luaState, "Keys");

	lua_createtable(luaState, 0, 14);
	lua_pushnumber(luaState, LcJKeys::X);
	lua_setfield(luaState, -2, "X");
	lua_pushnumber(luaState, LcJKeys::A);
	lua_setfield(luaState, -2, "A");
	lua_pushnumber(luaState, LcJKeys::B);
	lua_setfield(luaState, -2, "B");
	lua_pushnumber(luaState, LcJKeys::Y);
	lua_setfield(luaState, -2, "Y");
	lua_pushnumber(luaState, LcJKeys::L1);
	lua_setfield(luaState, -2, "L1");
	lua_pushnumber(luaState, LcJKeys::R1);
	lua_setfield(luaState, -2, "R1");
	lua_pushnumber(luaState, LcJKeys::L2);
	lua_setfield(luaState, -2, "L2");
	lua_pushnumber(luaState, LcJKeys::R2);
	lua_setfield(luaState, -2, "R2");
	lua_pushnumber(luaState, LcJKeys::Back);
	lua_setfield(luaState, -2, "Back");
	lua_pushnumber(luaState, LcJKeys::Menu);
	lua_setfield(luaState, -2, "Menu");
	lua_pushnumber(luaState, LcJKeys::Start);
	lua_setfield(luaState, -2, "Start");
	lua_pushnumber(luaState, LcJKeys::Up);
	lua_setfield(luaState, -2, "Up");
	lua_pushnumber(luaState, LcJKeys::Right);
	lua_setfield(luaState, -2, "Right");
	lua_pushnumber(luaState, LcJKeys::Down);
	lua_setfield(luaState, -2, "Down");
	lua_pushnumber(luaState, LcJKeys::Left);
	lua_setfield(luaState, -2, "Left");
	lua_setglobal(luaState, "JKeys");
}
