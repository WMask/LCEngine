/**
* LuaScriptSystem.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/ScriptSystem.h"


/**
* Lua script system */
class LCLUA_API LcLuaScriptSystem : public IScriptSystem
{
public:
	/**
	* Constructor */
	LcLuaScriptSystem(bool openBaseDefaultLibs = true, bool openAllDefaultLibs = false);


public: // IScriptSystem interface implementation
	/**
	* Destructor */
	~LcLuaScriptSystem();
	/**
	* Runs script */
	virtual void RunScript(const std::string& script) override { RunScriptEx(script); }
	/**
	* Runs script and return value */
	virtual LcAny RunScriptEx(const std::string& script) override;


public:
	LcLuaScriptSystem(const LcLuaScriptSystem&) = delete;
	LcLuaScriptSystem& operator=(const LcLuaScriptSystem&) = delete;


protected:
	struct lua_State* luaState;

};
