/**
* ScriptSystem.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/InputSystem.h"

#include <string>


/** App handlers */
enum class LcScriptHandler : int { Update, Actions, Keys, Axis };


/**
* Script system */
class IScriptSystem
{
public:
	/**
	* Virtual destructor */
	virtual ~IScriptSystem() {}
	/**
	* Init script system */
	virtual void Init(const struct LcAppContext& context) = 0;
	/**
	* Runs script */
	virtual void RunScript(const std::string& script) = 0;
	/**
	* Runs script from file */
	virtual void RunScriptFile(const char* filePath) = 0;
	/**
	* Runs script and return value */
	virtual LcAny RunScriptEx(const std::string& script) = 0;
	/**
	* Runs script from file and return value */
	virtual LcAny RunScriptFileEx(const char* filePath) = 0;
	/**
	* Runs Update script handler */
	virtual void RunUpdateHandler(float deltaSeconds) = 0;
	/**
	* Runs Update script handler */
	virtual void RunActionsHandler(const std::string& action) = 0;
	/**
	* Runs Update script handler */
	virtual void RunKeysHandler(int key, LcKeyState keyEvent) = 0;
	/**
	* Runs Update script handler */
	virtual void RunAxisHandler(int axis, float x, float y) = 0;
	/**
	* Set script handler function name */
	virtual void SetHandlerName(LcScriptHandler type, const char* name) = 0;
	/**
	* Get script handler function name */
	virtual const char* GetHandlerName(LcScriptHandler type) const = 0;

};


constexpr const char* LuaScriptGlobalName = "script";
constexpr const char* LuaAppGlobalName = "app";
constexpr const char* LuaWorldGlobalName = "world";
constexpr const char* LuaPhysicsGlobalName = "physics";
constexpr const char* LuaAudioGlobalName = "audio";
constexpr const char* LuaInputGlobalName = "input";
