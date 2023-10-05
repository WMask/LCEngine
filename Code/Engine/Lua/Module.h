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
* - void RequestQuit()
*/
LCLUA_API void AddLuaModuleApplication(const LcAppContext& context, IScriptSystem* scriptSystem = nullptr);

/**
* @brief Add World functions to script system.
* Functions:
*
* - ISprite* AddSprite(float x, float y, [optional { float z },] float width, float height, float rotation, bool visible)
*
* - IWidget* AddWidget(float x, float y, [optional { float z },] float width, float height, bool visible)
*
* - void AddTintComponent([optional IVisual* visual,] LcColor4 tint)
*
*	LcColor4 -> { r = 1.0, g = 0.0, b = 0.0, a = 1.0 }
* - void AddColorsComponent([optional IVisual* visual,] LcColor4 leftTop, LcColor4 rightTop, LcColor4 rightBottom, LcColor4 leftBottom)
*
* - void AddTextureComponent([optional IVisual* visual,] string texPath)
*
*	LcVector2 -> { x = 1.0, y = 1.0 }
* - void AddCustomUVComponent([optional ISprite* sprite,] LcVector2 leftTop, LcVector2 rightTop, LcVector2 rightBottom, LcVector2 leftBottom)
*
* - void AddAnimationComponent([optional ISprite* sprite,] LcVector2 frameSize, int numFrames, float framesPerSecond)
*
* - void AddTiledComponent([optional ISprite* sprite,] string tilesPath)
*
*	LcBasicParticleSettings -> {
*		frameSize = { x = 32.0, y = 32.0 },
*		numFrames = 8,
*		movementRadius = 10.0,
*		lifetime = 3.14,
*		fadeInRate = 0.3,
*		fadeOutRate = 0.3,
*		speed = 0.2
*	}
* - void AddParticlesComponent([optional ISprite* sprite,] int numSprites, LcBasicParticleSettings settings)
*
*	LcTextBlockSettings -> {
*		textColor = { r = 1.0, g = 0.0, b = 0.0 },
*		textAlign = "Center",
*		fontName = "Calibri",
*		fontWeight = "Normal",
*		fontSize = 20
*	}
* - void AddTextComponent([optional IWidget* widget,] string text, LcTextBlockSettings settings)
*
* - void AddButtonComponent([optional ISprite* sprite,] string texPath, LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos)
*
* - void AddCheckboxComponent([optional ISprite* sprite,] string texPath, LcVector2 uncheckedPos, LcVector2 uncheckedHoveredPos, LcVector2 checkedPos, LcVector2 checkedHoveredPos)
*
* - void AddClickHandlerComponent([optional ISprite* sprite,] string handlerFuncName)
*
* - void AddCheckHandlerComponent([optional ISprite* sprite,] string handlerFuncName)
*/
LCLUA_API void AddLuaModuleWorld(const LcAppContext& context, IScriptSystem* scriptSystem = nullptr);

/**
* @brief Add Audio functions to script system.
* Functions:
* - ISound* AddSound(string filePath)
*
* - void PlaySound(ISound* sound)
*
* - void PauseSound(ISound* sound)
*
* - void StopSound(ISound* sound)
*
* - void SetSoundVolume(ISound* sound, float volume)
*/
LCLUA_API void AddLuaModuleAudio(const LcAppContext& context, IScriptSystem* scriptSystem = nullptr);
