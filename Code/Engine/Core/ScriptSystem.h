/**
* ScriptSystem.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypes.h"

#include <string>


/**
* Script system */
class IScriptSystem
{
public:
	/**
	* Virtual destructor */
	virtual ~IScriptSystem() {}
	/**
	* Runs script */
	virtual void RunScript(const std::string& script) = 0;
	/**
	* Runs script and return value */
	virtual LcAny RunScriptEx(const std::string& script) = 0;

};


/**
* Script module */
class IScriptModule
{
public:
	/**
	* Virtual destructor */
	virtual ~IScriptModule() {}
	/**
	* Adds script module */
	virtual void AddTo(IScriptSystem& system) = 0;

};
