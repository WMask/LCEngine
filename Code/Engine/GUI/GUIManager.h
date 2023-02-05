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
	virtual ~IGuiManager() {}
	/**
	* Init GUI manager */
	virtual void Init(void* window, LcSize viewportSize);
	/**
	* Update GUI */
	virtual void Update(float DeltaSeconds);
	/**
	* Render GUI */
	virtual void Render() = 0;
	/**
	* Shutdown GUI manager */
	virtual void Shutdown() = 0;
	/**
	* Keyboard key event */
	virtual void OnKeyboard(int btn, LcKeyState state);
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y);
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y);


protected:
	/**
	* Render GUI */
	virtual void PreRender();
	/**
	* Render GUI */
	virtual void PostRender();

};
