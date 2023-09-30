/**
* LuaScriptSystem.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/ScriptSystem.h"
#include "World/WorldInterface.h"
#include "Application/ApplicationInterface.h"

#pragma warning(disable : 4275)

#define lua_tofloat(L,i) (float)lua_tonumberx(L,(i),NULL)

LCLUA_API IApplication* GetApp(struct lua_State* luaState);
LCLUA_API IWorld* GetWorld(struct lua_State* luaState);


/**
* Lua script system */
class LCLUA_API LcLuaScriptSystem : public IScriptSystem
{
public:
	//
	LcLuaScriptSystem(bool openBaseDefaultLibs = true, bool openAllDefaultLibs = false);
	//
	struct lua_State* GetState() { return luaState; }


public: // IScriptSystem interface implementation
	//
	~LcLuaScriptSystem();
	//
	virtual void RunScript(const std::string& script) override;
	//
	virtual void RunScriptFile(const char* filePath) override;
	//
	virtual LcAny RunScriptEx(const std::string& script) override;
	//
	virtual LcAny RunScriptFileEx(const char* filePath) override;


public:
	LcLuaScriptSystem(const LcLuaScriptSystem&) = delete;
	LcLuaScriptSystem& operator=(const LcLuaScriptSystem&) = delete;


protected:
	struct lua_State* luaState;

};
