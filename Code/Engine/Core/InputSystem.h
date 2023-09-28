/**
* InputSystem.h
* 21.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypes.h"

#include <functional>

#pragma warning(disable : 4275)
#pragma warning(disable : 4251)


constexpr int LcJoysticksMaxCount = 4;
constexpr int LcJoystickKeysOffset = 130;
constexpr int LcJoystickKeysCount = (LcKeysCount - LcJoystickKeysOffset);

/** Input device type */
enum class LcInputDeviceType : int {
	Keyboard,
	Joystick1,
	Joystick2,
	Joystick3,
	Joystick4
};

/** Joystick keys */
namespace LcJKeys
{
	constexpr int X		= LcJoystickKeysOffset + 0;
	constexpr int A		= LcJoystickKeysOffset + 1;
	constexpr int B		= LcJoystickKeysOffset + 2;
	constexpr int Y		= LcJoystickKeysOffset + 3;
	constexpr int L1	= LcJoystickKeysOffset + 4;
	constexpr int R1	= LcJoystickKeysOffset + 5;
	constexpr int L2	= LcJoystickKeysOffset + 6;
	constexpr int R2	= LcJoystickKeysOffset + 7;
	constexpr int Back	= LcJoystickKeysOffset + 8;
	constexpr int Menu	= LcJoystickKeysOffset + 9;
	constexpr int Start	= LcJoystickKeysOffset + 9;
	constexpr int Up	= LcJoystickKeysOffset + 10;
	constexpr int Right	= LcJoystickKeysOffset + 11;
	constexpr int Down	= LcJoystickKeysOffset + 12;
	constexpr int Left	= LcJoystickKeysOffset + 13;
	constexpr int StartArrows = Up;
	constexpr int EndArrows = Left;
}

/** Joystick axis */
namespace LcJAxis
{
	constexpr int LStick = 0;
	constexpr int RStick = 1;
}


/**
* Input device interface */
class IInputDevice
{
public:
	/**
	* Virtual destructor */
	virtual ~IInputDevice() {}
	/**
	* Set as active */
	virtual void Activate() = 0;
	/**
	* Set as inactive */
	virtual void Deactivate() = 0;
	/**
	* Get active state */
	virtual bool IsActive() const = 0;
	/**
	* Get joystick name */
	virtual std::wstring GetName() const = 0;
	/**
	* Get input state */
	virtual const KEYS& GetState() const = 0;
	/**
	* Get input state */
	virtual KEYS& GetState() = 0;
	/**
	* Get joystick type */
	virtual LcInputDeviceType GetType() const = 0;

};


/** Input devices list */
typedef std::deque<std::shared_ptr<IInputDevice>> TInputDevicesList;

/** Keyboard events handler */
typedef std::function<void(int, LcKeyState, const struct LcAppContext&)> LcKeysHandler;

/** Gamepad axis events handler */
typedef std::function<void(int, float, float, const struct LcAppContext&)> LcAxisHandler;

/** Mouse button handler */
typedef std::function<void(LcMouseBtn, LcKeyState, float, float, const struct LcAppContext&)> LcMouseButtonHandler;

/** Mouse move handler */
typedef std::function<void(float, float, const struct LcAppContext&)> LcMouseMoveHandler;


/**
* Input system */
class IInputSystem
{
public:
	/**
	* Virtual destructor */
	virtual ~IInputSystem() {}
	/**
	* Initialize input system */
	virtual void Init(const struct LcAppContext& context) = 0;
	/**
	* Shutdown input system */
	virtual void Shutdown() = 0;
	/**
	* Update input system */
	virtual void Update(float deltaSeconds, const struct LcAppContext& context) = 0;
	/**
	* Set active device */
	virtual void SetActiveDevice(const IInputDevice* device) = 0;
	/**
	* Set active device by name part. Like "MyGamepad" for "Wireless MyGamepad Controller" */
	virtual void SetActiveDevice(const std::wstring& deviceNamePart) = 0;
	/**
	* Set keyboard handler */
	virtual void SetKeysHandler(LcKeysHandler handler) noexcept = 0;
	/**
	* Set gamepad axis handler */
	virtual void SetAxisHandler(LcAxisHandler handler) noexcept = 0;
	/**
	* Set mouse move handler */
	virtual void SetMouseMoveHandler(LcMouseMoveHandler handler) noexcept = 0;
	/**
	* Set mouse button handler */
	virtual void SetMouseButtonHandler(LcMouseButtonHandler handler) noexcept = 0;
	/**
	* Get keyboard handler */
	virtual LcKeysHandler& GetKeysHandler() noexcept = 0;
	/**
	* Get gamepad axis handler */
	virtual LcAxisHandler& GetAxisHandler() noexcept = 0;
	/**
	* Get mouse move handler */
	virtual LcMouseMoveHandler& GetMouseMoveHandler() noexcept = 0;
	/**
	* Get mouse button handler */
	virtual LcMouseButtonHandler& GetMouseButtonHandler() noexcept = 0;
	/**
	* Get input devices list */
	virtual const TInputDevicesList& GetInputDevicesList() const = 0;
	/**
	* Get input devices list */
	virtual TInputDevicesList& GetInputDevicesList() = 0;
	/**
	* Get active input device */
	virtual const IInputDevice* GetActiveInputDevice() const = 0;
	/**
	* Get active input device */
	virtual IInputDevice* GetActiveInputDevice() = 0;

};


/**
* Default input device */
class CORE_API LcDefaultInputDevice : public IInputDevice
{
public:
	static const std::wstring Name;


public:
	LcDefaultInputDevice() : name(Name), active(true) {}


public:
	//
	virtual ~LcDefaultInputDevice() {}
	//
	virtual void Activate() override { active = true; }
	//
	virtual void Deactivate() override { active = false; }
	//
	virtual bool IsActive() const override { return active; }
	//
	virtual std::wstring GetName() const override { return name; }
	//
	virtual const KEYS& GetState() const override { return keys; }
	//
	virtual KEYS& GetState() override { return keys; }
	//
	virtual LcInputDeviceType GetType() const override { return LcInputDeviceType::Keyboard; }


protected:
	KEYS keys;
	//
	std::wstring name;
	//
	bool active;

};


/**
* Default Input system */
class CORE_API LcDefaultInputSystem : public IInputSystem
{
public:
	//
	LcDefaultInputSystem();


public: // IInputSystem interface implementation
	//
	virtual ~LcDefaultInputSystem() override {}
	//
	virtual void Init(const struct LcAppContext& inContext) override {}
	//
	virtual void Shutdown() override {}
	//
	virtual void Update(float deltaSeconds, const struct LcAppContext& context) override {}
	//
	virtual void SetActiveDevice(const std::wstring& deviceNamePart) override;
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
* Default input system */
CORE_API TInputSystemPtr GetDefaultInputSystem();
