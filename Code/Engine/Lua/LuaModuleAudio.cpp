/**
* LuaModuleAudio.cpp
* 04.10.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
#include "Core/LCUtils.h"
#include "Core/Audio.h"

#include "src/lua.hpp"


static int AddSound(lua_State* luaState)
{
	int top = lua_gettop(luaState);
	if (!lua_isstring(luaState, top))
	{
		throw std::exception("AddSound(): Invalid path");
	}

	auto app = GetApp(luaState);
	auto audio = app->GetContext().audio;
	if (!audio) throw std::exception("AddSound(): Invalid Audio system");

	auto filePath = lua_tostring(luaState, top);
	ISound* sound = audio->AddSound(filePath);
	lua_pushlightuserdata(luaState, sound);

	return 1;
}

static int PlaySound(lua_State* luaState)
{
	int top = lua_gettop(luaState);
	if (lua_isuserdata(luaState, top))
	{
		ISound* sound = static_cast<ISound*>(lua_touserdata(luaState, top));
		if (!sound) throw std::exception("PlaySound(): Invalid sound");

		sound->Play();
	}

	return 0;
}

static int PauseSound(lua_State* luaState)
{
	int top = lua_gettop(luaState);
	if (lua_isuserdata(luaState, top))
	{
		ISound* sound = static_cast<ISound*>(lua_touserdata(luaState, top));
		if (!sound) throw std::exception("PauseSound(): Invalid sound");

		sound->Pause();
	}

	return 0;
}

static int StopSound(lua_State* luaState)
{
	int top = lua_gettop(luaState);
	if (lua_isuserdata(luaState, top))
	{
		ISound* sound = static_cast<ISound*>(lua_touserdata(luaState, top));
		if (!sound) throw std::exception("StopSound(): Invalid sound");

		sound->Stop();
	}

	return 0;
}

static int SetSoundVolume(lua_State* luaState)
{
	int top = lua_gettop(luaState);
	if (lua_isuserdata(luaState, top - 1))
	{
		ISound* sound = static_cast<ISound*>(lua_touserdata(luaState, top - 1));
		if (!sound) throw std::exception("SetSoundVolume(): Invalid sound");

		sound->SetVolume(lua_tofloat(luaState, top));
	}

	return 0;
}


void AddLuaModuleAudio(const LcAppContext& context, IScriptSystem* scriptSystem)
{
	auto luaSystem = static_cast<LcLuaScriptSystem*>(context.scripts);
	auto luaSystemCustom = static_cast<LcLuaScriptSystem*>(scriptSystem);
	auto luaState = luaSystemCustom ? luaSystemCustom->GetState() : luaSystem->GetState();
	if (!luaState) throw std::exception("AddLuaModuleAudio(): Invalid Lua state");

	lua_pushcfunction(luaState, AddSound);
	lua_setglobal(luaState, "AddSound");

	lua_pushcfunction(luaState, PlaySound);
	lua_setglobal(luaState, "PlaySound");

	lua_pushcfunction(luaState, PauseSound);
	lua_setglobal(luaState, "PauseSound");

	lua_pushcfunction(luaState, StopSound);
	lua_setglobal(luaState, "StopSound");

	lua_pushcfunction(luaState, SetSoundVolume);
	lua_setglobal(luaState, "SetSoundVolume");
}
