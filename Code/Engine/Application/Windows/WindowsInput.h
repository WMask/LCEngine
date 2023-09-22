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
	LcWindowsInputSystem();


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
	virtual void SetActiveDevice(const IInputDevice* device) override;
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
	virtual const TInputDevicesList& GetInputDevicesList() const override { return devices; }
	//
	virtual TInputDevicesList& GetInputDevicesList() override { return devices; }
	//
	virtual const IInputDevice* GetActiveInputDevice() const override { return activeDevice; }
	//
	virtual IInputDevice* GetActiveInputDevice() override { return activeDevice; }


protected:
	//
	LcKeysHandler keysHandler;
	//
	LcAxisHandler axisHandler;
	//
	LcMouseMoveHandler mouseMoveHandler;
	//
	LcMouseButtonHandler mouseButtonHandler;
	//
	IInputDevice* activeDevice;
	//
	TInputDevicesList devices;

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
	ComPtr<IDirectInput8> directInput;

};


typedef ComPtr<IDirectInputDevice8> TDevicePtr;

/**
* DirectInput joystick */
class LcDirectInputJoystick : public LcKeyboard
{
public:
	LcDirectInputJoystick(const std::wstring& inName, int inDeviceId);
	//
	void SetButtonsState(const BYTE* inKeys);
	//
	void GetButtonsState(BYTE* inKeys) const;
	//
	void SetArrowsState(UINT inArrows) { arrows = inArrows; }
	//
	void GetArrowsState(UINT& inArrows) const { inArrows = arrows; }
	//
	TDevicePtr& GetDevice() { return device; }


public:
	//
	virtual ~LcDirectInputJoystick() {}
	//
	virtual void Activate() override;
	//
	virtual void Deactivate() override;
	//
	virtual LcInputDeviceType GetType() const override;


protected:
	//
	TDevicePtr device;
	//
	UINT arrows;
	//
	int deviceId;

};
