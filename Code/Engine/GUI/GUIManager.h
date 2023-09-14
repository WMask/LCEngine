/**
* GUIManager.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "World/Module.h"
#include "RenderSystem/RenderSystem.h"
#include "Core/LCTypesEx.h"


/**
* GUI manager */
class IGuiManager
{
public:
	/**
	* Destructor */
	virtual ~IGuiManager() {}
	/**
	* Shutdown GUI manager */
	virtual void Shutdown() = 0;
	/**
	* Update GUI */
	virtual void Update(float deltaSeconds, class IWorld& world) = 0;
	/**
	* Keyboard key event */
	virtual void OnKeyboard(int btn, LcKeyState state, class IWorld& world) = 0;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y, class IWorld& world) = 0;
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y, class IWorld& world) = 0;

};


/**
* GUI manager */
class LcGuiManager : public IGuiManager
{
public:
	/**
	* Constructor */
	LcGuiManager() {}
	/**
	* Shutdown GUI manager */
	virtual void Shutdown() override {}
	/**
	* Update GUI */
	virtual void Update(float deltaSeconds, class IWorld& world) override;
	/**
	* Keyboard key event */
	virtual void OnKeyboard(int btn, LcKeyState state, class IWorld& world) override;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y, class IWorld& world) override;
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y, class IWorld& world) override;

};
