/**
* GUIManager.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypes.h"


/**
* GUI manager */
class GUI_API IGuiManager
{
public:
	/**
	* Destructor */
	virtual ~IGuiManager();
	/**
	* Init GUI manager */
	virtual void Init(LcSize viewportSize) = 0;
	/**
	* Shutdown GUI manager */
	virtual void Shutdown() = 0;
	/**
	* Keyboard key event */
	virtual void OnKeyboard(int btn, LcKeyState state) = 0;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) = 0;
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y) = 0;

};
