/**
* ScriptSystem.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"

#include <string>


/**
* Script system */
class CORE_API IScriptSystem
{
public:
	/**
	* Destructor */
	virtual ~IScriptSystem();
	/**
	* Runs script */
	virtual void RunScript(const std::string& script) = 0;

};


/**
* Script module */
class CORE_API IScriptModule
{
public:
	/**
	* Destructor */
	virtual ~IScriptModule();
	/**
	* Adds script module */
	virtual void AddModule(IScriptSystem* system) = 0;

};
