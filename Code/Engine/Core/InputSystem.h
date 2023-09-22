/**
* InputSystem.h
* 21.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypes.h"

#include <functional>


#define LC_JOYSTICK_MAX_COUNT 4
#define LC_JOYSTICK_KEYS_OFFSET 200
#define LC_JOYSTICK_KEYS_COUNT (LC_KEYS_COUNT - LC_JOYSTICK_KEYS_OFFSET)

/** Input device type */
enum class LcInputDeviceType : int {
	Keyboard,
	Joystick1,
	Joystick2,
	Joystick3,
	Joystick4
};

/** Joystick keys */
enum class LcJKeys : int {
	X		= LC_JOYSTICK_KEYS_OFFSET + 0,
	A		= LC_JOYSTICK_KEYS_OFFSET + 1,
	B		= LC_JOYSTICK_KEYS_OFFSET + 2,
	Y		= LC_JOYSTICK_KEYS_OFFSET + 3,
	L1		= LC_JOYSTICK_KEYS_OFFSET + 4,
	R1		= LC_JOYSTICK_KEYS_OFFSET + 5,
	L2		= LC_JOYSTICK_KEYS_OFFSET + 6,
	R2		= LC_JOYSTICK_KEYS_OFFSET + 7,
	Back	= LC_JOYSTICK_KEYS_OFFSET + 8,
	Menu	= LC_JOYSTICK_KEYS_OFFSET + 9,
	Start	= LC_JOYSTICK_KEYS_OFFSET + 9,
	Up		= LC_JOYSTICK_KEYS_OFFSET + 10, StartArrows = Up,
	Right	= LC_JOYSTICK_KEYS_OFFSET + 11,
	Down	= LC_JOYSTICK_KEYS_OFFSET + 12,
	Left	= LC_JOYSTICK_KEYS_OFFSET + 13, EndArrows = Left
};


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
typedef std::function<void(int, LcKeyState, class IApplication*)> LcKeysHandler;

/** Gamepad axis events handler */
typedef std::function<void(int, float, float, class IApplication*)> LcAxisHandler;

/** Mouse button handler */
typedef std::function<void(LcMouseBtn, LcKeyState, float, float, class IApplication*)> LcMouseButtonHandler;

/** Mouse move handler */
typedef std::function<void(float, float, class IApplication*)> LcMouseMoveHandler;


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
	virtual void Init(struct LcAppContext& context) = 0;
	/**
	* Shutdown input system */
	virtual void Shutdown() = 0;
	/**
	* Update input system */
	virtual void Update(float deltaSeconds, struct LcAppContext& context) = 0;
	/**
	* Set active device */
	virtual void SetActiveDevice(const IInputDevice* device) = 0;
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
* Default keyboard */
class LcKeyboard : public IInputDevice
{
public:
	LcKeyboard() : name(L"Keyboard"), active(true) {}


public:
	//
	virtual ~LcKeyboard() {}
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
