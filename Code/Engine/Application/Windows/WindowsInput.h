/**
* WindowsInput.h
* 21.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/InputSystem.h"

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <wrl.h>

using namespace Microsoft::WRL;


/**
* Win32 Input system */
class LcWindowsInputSystem : public IInputSystem
{
public:
	//
	LcWindowsInputSystem() {}


public: // IInputSystem interface implementation
	//
	virtual ~LcWindowsInputSystem() override {}
	//
	virtual void Init(struct LcAppContext& context) override {}
	//
	virtual void Shutdown() override {}
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


/**
* DirectInput system */
class LcDirectInputSystem : public LcWindowsInputSystem
{
public:
	//
	LcDirectInputSystem();


public: // LcWindowsInputSystem override
	//
	virtual ~LcDirectInputSystem() override;
	//
	virtual void Init(struct LcAppContext& context) override;
	//
	virtual void Shutdown() override;
	//
	virtual void Update(float deltaSeconds, struct LcAppContext& context) override;


protected:
	//
	void EnumerateDevices();


protected:
	//
	ComPtr<IDirectInput8> directInput;

};
