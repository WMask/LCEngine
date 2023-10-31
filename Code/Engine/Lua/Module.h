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
*   LcTiledProps -> { string name, any value }
*   Handler -> void (string layerName, string objName, string objType, LcTiledProps objProps, LcVector2 objPos, LcSizef objSize)
* - void AddTiledComponent([optional ISprite* sprite,] string tilesPath, string objectsHandlerName)
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
* - void AddTextComponent([optional IWidget* widget,] string textKey, LcTextBlockSettings settings)
*
* - void AddButtonComponent([optional ISprite* sprite,] string texPath, LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos)
*
* - void AddCheckboxComponent([optional ISprite* sprite,] string texPath, LcVector2 uncheckedPos, LcVector2 uncheckedHoveredPos, LcVector2 checkedPos, LcVector2 checkedHoveredPos)
*
* - void AddClickHandlerComponent([optional ISprite* sprite,] string handlerFuncName)
*
* - void AddCheckHandlerComponent([optional ISprite* sprite,] string handlerFuncName)
*
* - void SetVisualPos(ISprite* sprite, LcVector3 pos)
*
* - LcVector3 GetVisualPos(ISprite* sprite)
*
* - IVisual* GetVisualByTag(int tag)
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
*
* - ISound* GetSoundByTag(int tag)
*/
LCLUA_API void AddLuaModuleAudio(const LcAppContext& context, IScriptSystem* scriptSystem = nullptr);

/**
* @brief Add Audio functions to script system.
* Functions:
* - void AddStaticBox(string filePath)
*
* - IPhysicsBody* AddDynamic(LcVector2 pos, float radius, float density, bool fixedRotation)
*
*	LcSizef -> { x = 10.0, y = 10.0 }
* - IPhysicsBody* AddDynamicBox(LcVector2 pos, LcSizef size, float density, bool fixedRotation)
*
* - void ApplyBodyImpulse(IPhysicsBody* body, LcVector2 impulse)
*
* - void SetBodyPos(IPhysicsBody* body, LcVector2 pos)
*
* - LcVector2 GetBodyPos(IPhysicsBody* body)
*
* - void SetBodyVelocity(IPhysicsBody* body, LcVector2 velocity)
*
* - LcVector2 GetBodyVelocity(IPhysicsBody* body)
*
* - bool InBodyFalling(IPhysicsBody* body)
*
* - void SetBodyUserData(IPhysicsBody* body, void* userData)
*
* - IPhysicsBody* GetBodyByTag(int tag)
*/
LCLUA_API void AddLuaModulePhysics(const LcAppContext& context, IScriptSystem* scriptSystem = nullptr);

/**
* @brief Add Audio functions to script system.
* Functions:
*
* - bool IsKeyPressed(int key)
*/
LCLUA_API void AddLuaModuleInput(const LcAppContext& context, IScriptSystem* scriptSystem = nullptr);
