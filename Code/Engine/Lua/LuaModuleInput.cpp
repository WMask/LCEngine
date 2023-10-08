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
	lua_pushinteger(luaState, 0);
	lua_setfield(luaState, -2, "Up");
	lua_pushinteger(luaState, 1);
	lua_setfield(luaState, -2, "Down");
	lua_setglobal(luaState, "KeyState");

	lua_createtable(luaState, 0, 8);
	lua_pushinteger(luaState, LcKeys::Escape);
	lua_setfield(luaState, -2, "Escape");
	lua_pushinteger(luaState, LcKeys::Space);
	lua_setfield(luaState, -2, "Space");
	lua_pushinteger(luaState, LcKeys::Return);
	lua_setfield(luaState, -2, "Return");
	lua_pushinteger(luaState, LcKeys::Enter);
	lua_setfield(luaState, -2, "Enter");
	lua_pushinteger(luaState, LcKeys::Up);
	lua_setfield(luaState, -2, "Up");
	lua_pushinteger(luaState, LcKeys::Right);
	lua_setfield(luaState, -2, "Right");
	lua_pushinteger(luaState, LcKeys::Down);
	lua_setfield(luaState, -2, "Down");
	lua_pushinteger(luaState, LcKeys::Left);
	lua_setfield(luaState, -2, "Left");
	lua_setglobal(luaState, "Keys");

	lua_createtable(luaState, 0, 14);
	lua_pushinteger(luaState, LcJKeys::X);
	lua_setfield(luaState, -2, "X");
	lua_pushinteger(luaState, LcJKeys::A);
	lua_setfield(luaState, -2, "A");
	lua_pushinteger(luaState, LcJKeys::B);
	lua_setfield(luaState, -2, "B");
	lua_pushinteger(luaState, LcJKeys::Y);
	lua_setfield(luaState, -2, "Y");
	lua_pushinteger(luaState, LcJKeys::L1);
	lua_setfield(luaState, -2, "L1");
	lua_pushinteger(luaState, LcJKeys::R1);
	lua_setfield(luaState, -2, "R1");
	lua_pushinteger(luaState, LcJKeys::L2);
	lua_setfield(luaState, -2, "L2");
	lua_pushinteger(luaState, LcJKeys::R2);
	lua_setfield(luaState, -2, "R2");
	lua_pushinteger(luaState, LcJKeys::Back);
	lua_setfield(luaState, -2, "Back");
	lua_pushinteger(luaState, LcJKeys::Menu);
	lua_setfield(luaState, -2, "Menu");
	lua_pushinteger(luaState, LcJKeys::Start);
	lua_setfield(luaState, -2, "Start");
	lua_pushinteger(luaState, LcJKeys::Up);
	lua_setfield(luaState, -2, "Up");
	lua_pushinteger(luaState, LcJKeys::Right);
	lua_setfield(luaState, -2, "Right");
	lua_pushinteger(luaState, LcJKeys::Down);
	lua_setfield(luaState, -2, "Down");
	lua_pushinteger(luaState, LcJKeys::Left);
	lua_setfield(luaState, -2, "Left");
	lua_setglobal(luaState, "JKeys");
}
