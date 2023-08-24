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
	LcApplicationLuaModule(std::weak_ptr<class IApplication> appPtr);


public: // IScriptModule interface implementation
	/**
	* Virtual destructor */
	virtual ~LcApplicationLuaModule() override;
	/**
	* Adds script module */
	virtual void Add(IScriptSystem& system) override;


protected:
	std::weak_ptr<IApplication> weakApp;

};


LCLUA_API IScriptModule* GetApplicationLuaModule(std::weak_ptr<class IApplication> appPtr);
