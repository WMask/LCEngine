/**
* ApplicationLuaModule.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/ScriptSystem.h"

#include <memory>


/**
* Application Lua module */
class LcApplicationLuaModule : public IScriptModule
{
public:
	/**
	* Default constructor */
	LcApplicationLuaModule(class IApplication& app);


public: // IScriptModule interface implementation
	/**
	* Virtual destructor */
	virtual ~LcApplicationLuaModule() override;
	/**
	* Adds script module */
	virtual void Add(IScriptSystem& system) override;


protected:
	IApplication& app;

};


LCLUA_API IScriptModule& GetApplicationLuaModule(class IApplication& app);
