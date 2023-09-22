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

enum LcInputDeviceType {
	Keyboard,
	Joystick1,
	Joystick2,
	Joystick3,
	Joystick4
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
	virtual std::string GetName() const = 0;
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
	/**
	* Get input state */
	virtual const KEYS& GetState() const = 0;
	/**
	* Get input state */
	virtual KEYS& GetState() = 0;

};


/**
* Default keyboard */
class LcKeyboard : public IInputDevice
{
public:
	LcKeyboard() : active(true) {}


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
	virtual std::string GetName() const override { return "Keyboard"; }
	//
	virtual LcInputDeviceType GetType() const override { return LcInputDeviceType::Keyboard; }


protected:
	bool active;

};
