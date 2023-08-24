/**
* GUIManager.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "World/Module.h"
#include "Core/LCTypes.h"


/**
* GUI manager */
class IGuiManager
{
public:
	/**
	* Destructor */
	virtual ~IGuiManager() {}
	/**
	* Init GUI manager */
	virtual void Init(TWorldWeakPtr world, void* window) = 0;
	/**
	* Update GUI */
	virtual void Update(float DeltaSeconds) = 0;
	/**
	* Render GUI */
	virtual void Render() = 0;
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


/**
* GUI manager base */
class LcGuiManagerBase : public IGuiManager
{
public:
	/**
	* Constructor */
	LcGuiManagerBase() {}
	/**
	* Init GUI manager */
	virtual void Init(TWorldWeakPtr world, void* window);
	/**
	* Update GUI */
	virtual void Update(float DeltaSeconds);
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


protected:
	TWorldWeakPtr worldPtr;

};
