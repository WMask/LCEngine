/**
* ApplicationLuaModule.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "Lua/ApplicationLuaModule.h"
#include "Lua/LuaScriptSystem.h"
#include "Application/Application.h"


LcApplicationLuaModule::LcApplicationLuaModule(std::weak_ptr<IApplication> appPtr) : weakApp(appPtr)
{
}

LcApplicationLuaModule::~LcApplicationLuaModule()
{
}

void LcApplicationLuaModule::AddModule(IScriptSystem* system)
{
	auto luaSystem = (LcLuaScriptSystem*)system;
	if (!luaSystem) throw std::exception("LcApplicationLuaModule::AddModule(): Invalid script system");
	if (auto app = weakApp.lock())
	{

	}
	else
		throw std::exception("LcApplicationLuaModule::AddModule(): Invalid Application pointer");
}

IScriptModule* GetApplicationLuaModule(std::weak_ptr<IApplication> appPtr)
{
	static LcApplicationLuaModule instance(appPtr);
	return &instance;
}
