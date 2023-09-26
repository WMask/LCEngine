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
* DirectInput system */
class LcDirectInputSystem : public LcDefaultInputSystem
{
public:
	//
	LcDirectInputSystem();


public: // LcWindowsInputSystem override
	//
	virtual ~LcDirectInputSystem() override;
	//
	virtual void Init(const struct LcAppContext& context) override;
	//
	virtual void Shutdown() override;
	//
	virtual void Update(float deltaSeconds, const struct LcAppContext& context) override;


protected:
	//
	ComPtr<IDirectInput8> directInput;

};


typedef ComPtr<IDirectInputDevice8> TDevicePtr;

/**
* DirectInput joystick */
class LcDirectInputJoystick : public LcDefaultInputDevice
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
