/**
* GUIManager.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "World/Module.h"
#include "RenderSystem/RenderSystem.h"
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
	virtual void Init(TWeakWorld world, TWeakWidgetRender render, void* window) = 0;
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
	virtual void Init(TWeakWorld world, TWeakWidgetRender render, void* window) override;
	/**
	* Update GUI */
	virtual void Update(float DeltaSeconds) override;
	/**
	* Render GUI */
	virtual void Render() override;
	/**
	* Shutdown GUI manager */
	virtual void Shutdown() override {}
	/**
	* Keyboard key event */
	virtual void OnKeyboard(int btn, LcKeyState state) override;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) override;
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y) override;


protected:
	TWeakWorld worldPtr;
	//
	TWeakWidgetRender renderPtr;

};
