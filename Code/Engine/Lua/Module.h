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
LCLUA_API void AddLuaModuleApplication(const LcAppContext& context, IScriptSystem* scriptSystem = nullptr);

/**
* @brief Add World functions to script system.
* Functions:
* - ISprite* AddSprite(float x, float y, [optional { float z },] float width, float height, float rotation, bool visible)
*/
LCLUA_API void AddLuaModuleWorld(const LcAppContext& context, IScriptSystem* scriptSystem = nullptr);

/**
* @brief Add Audio functions to script system.
* Functions:
* - ISound* AddSound(string filePath)
* - void PlaySound(ISound* sound)
* - void PauseSound(ISound* sound)
* - void StopSound(ISound* sound)
*/
LCLUA_API void AddLuaModuleAudio(const LcAppContext& context, IScriptSystem* scriptSystem = nullptr);
