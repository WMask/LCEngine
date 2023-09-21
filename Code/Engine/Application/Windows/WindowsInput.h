/**
* WindowsInput.h
* 21.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/InputSystem.h"


/**
* Win32 Input system */
class LcInputSystemWindows : public IInputSystem
{
public:
	//
	LcInputSystemWindows() {}


public: // IInputSystem interface implementation
	//
	virtual ~LcInputSystemWindows() override {}
	//
	virtual void Init(struct LcAppContext& context) override {}
	//
	virtual void Update(float deltaSeconds, struct LcAppContext& context) override {}
	//
	virtual void SetKeysHandler(LcKeysHandler handler) noexcept override { keysHandler = handler; }
	//
	virtual void SetAxisHandler(LcAxisHandler handler) noexcept override { axisHandler = handler; }
	//
	virtual void SetMouseMoveHandler(LcMouseMoveHandler handler) noexcept override { mouseMoveHandler = handler; }
	//
	virtual void SetMouseButtonHandler(LcMouseButtonHandler handler) noexcept override { mouseButtonHandler = handler; }
	//
	virtual LcKeysHandler& GetKeysHandler() noexcept override { return keysHandler; }
	//
	virtual LcAxisHandler& GetAxisHandler() noexcept override { return axisHandler; }
	//
	virtual LcMouseMoveHandler& GetMouseMoveHandler() noexcept override { return mouseMoveHandler; }
	//
	virtual LcMouseButtonHandler& GetMouseButtonHandler() noexcept override { return mouseButtonHandler; }
	//
	virtual const KEYS& GetState() const override { return keys; }
	//
	virtual KEYS& GetState() override { return keys; }


protected:
	//
	KEYS keys;
	//
	LcKeysHandler keysHandler;
	//
	LcAxisHandler axisHandler;
	//
	LcMouseMoveHandler mouseMoveHandler;
	//
	LcMouseButtonHandler mouseButtonHandler;

};
