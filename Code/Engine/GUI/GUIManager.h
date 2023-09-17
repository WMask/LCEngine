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
	virtual void Update(float deltaSeconds, const LcAppContext& context) = 0;
	/**
	* Keyboard key event */
	virtual void OnKeyboard(int btn, LcKeyState state, const LcAppContext& context) = 0;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y, const LcAppContext& context) = 0;
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y, const LcAppContext& context) = 0;

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
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	/**
	* Keyboard key event */
	virtual void OnKeyboard(int btn, LcKeyState state, const LcAppContext& context) override;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y, const LcAppContext& context) override;
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y, const LcAppContext& context) override;

};
