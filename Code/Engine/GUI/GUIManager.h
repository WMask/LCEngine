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


GUI_API bool HasInvisibleParent(class IWidget* widget);


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
	* Keyboard key event */
	virtual void OnKeys(int btn, LcKeyState state, const LcAppContext& context) = 0;
	/**
	* Mouse button event */
	virtual void OnMouseButton(int btn, LcKeyState state, int x, int y, const LcAppContext& context) = 0;
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
	* Keyboard key event */
	virtual void OnKeys(int btn, LcKeyState state, const LcAppContext& context) override;
	/**
	* Mouse button event */
	virtual void OnMouseButton(int btn, LcKeyState state, int x, int y, const LcAppContext& context) override;
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y, const LcAppContext& context) override;

};
