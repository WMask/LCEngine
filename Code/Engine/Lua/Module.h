/**
* Module.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/ScriptSystem.h"


#ifndef LUA_EXPORTS
#define LCLUA_API __declspec (dllimport)
#else
#define LCLUA_API __declspec (dllexport)
#endif


/**
* Lua script system */
LCLUA_API TScriptSystemPtr GetScriptSystem();


/**
* @brief Add Application functions to script system.
* Functions:
* - RequestQuit()
*/
LCLUA_API void AddLuaModuleApplication(const LcAppContext& context);
