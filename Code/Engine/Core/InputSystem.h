/**
* InputSystem.h
* 21.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypesEx.h"

#include <mutex>
#include <functional>

#ifdef _WINDOWS
# include <windows.h>
#endif

#pragma warning(disable : 4275)
#pragma warning(disable : 4251)


static const int LcKeysCount = 150;
static const int LcJoysticksMaxCount = 4;
static const int LcJoystickKeysOffset = 130;
static const int LcMouseKeysCount = 3;
constexpr int LcJoystickKeysCount = (LcKeysCount - LcJoystickKeysOffset);


/** Input device type */
enum class LcInputDeviceType : uint8_t
{
	Keyboard,
	Mouse,
	Joystick1,
	Joystick2,
	Joystick3,
	Joystick4
};

static constexpr uint8_t FirstJoystickId = static_cast<uint8_t>(LcInputDeviceType::Joystick1);

/** Keys struct */
struct CORE_API KEYS
{
	KEYS();
	//
	uint8_t* Get() { return keys; }
	//
	const uint8_t* Get() const { return keys; }
	// throws
	uint8_t& at(int index);
	//
	uint8_t& operator[](int index) noexcept { return keys[index]; }
	//
	uint8_t keys[LcKeysCount];
};

/** Mouse buttons */
namespace LcMouseBtn
{
	static const int Left   = 0;
	static const int Right  = 1;
	static const int Middle = 2;
}

/** Keyboard keys */
namespace LcKeys
{
#ifdef _WINDOWS

	static const int Escape = VK_ESCAPE;
	static const int Space  = VK_SPACE;
	static const int Return = VK_RETURN;
	static const int Enter  = VK_RETURN;
	static const int Up     = VK_UP;
	static const int Right  = VK_RIGHT;
	static const int Down   = VK_DOWN;
	static const int Left   = VK_LEFT;

#endif
}

/** Joystick keys */
namespace LcJKeys
{
	static const int X				= LcJoystickKeysOffset + 0;
	static const int A				= LcJoystickKeysOffset + 1;
	static const int B				= LcJoystickKeysOffset + 2;
	static const int Y				= LcJoystickKeysOffset + 3;
	static const int L1				= LcJoystickKeysOffset + 4;
	static const int R1				= LcJoystickKeysOffset + 5;
	static const int L2				= LcJoystickKeysOffset + 6;
	static const int R2				= LcJoystickKeysOffset + 7;
	static const int Back			= LcJoystickKeysOffset + 8;
	static const int Menu			= LcJoystickKeysOffset + 9;
	static const int Start			= LcJoystickKeysOffset + 9;
	static const int Up				= LcJoystickKeysOffset + 10;
	static const int Right			= LcJoystickKeysOffset + 11;
	static const int Down			= LcJoystickKeysOffset + 12;
	static const int Left			= LcJoystickKeysOffset + 13;
	static const int StartArrows	= Up;
	static const int EndArrows		= Left;
}

/** Joystick axis */
namespace LcJAxis
{
	static const int LStick = 0;
	static const int RStick = 1;
}

/** Joystick keys */
namespace LcMouseKeys
{
	static const int X           = LcJoystickKeysOffset + 0;
	static const int A           = LcJoystickKeysOffset + 1;
	static const int B           = LcJoystickKeysOffset + 2;
	static const int Y           = LcJoystickKeysOffset + 3;
	static const int L1          = LcJoystickKeysOffset + 4;
	static const int R1          = LcJoystickKeysOffset + 5;
	static const int L2          = LcJoystickKeysOffset + 6;
	static const int R2          = LcJoystickKeysOffset + 7;
	static const int Back        = LcJoystickKeysOffset + 8;
	static const int Menu        = LcJoystickKeysOffset + 9;
	static const int Start       = LcJoystickKeysOffset + 9;
	static const int Up          = LcJoystickKeysOffset + 10;
	static const int Right       = LcJoystickKeysOffset + 11;
	static const int Down        = LcJoystickKeysOffset + 12;
	static const int Left        = LcJoystickKeysOffset + 13;
	static const int StartArrows = Up;
	static const int EndArrows   = Left;
}


/** Action type */
enum class LcActionType
{
	Key,
	Mouse,
	Axis
};

/** Action base */
struct LcAction
{
	LcAction(const std::string& inName, LcActionType inType, LcKeyState inState = LcKeyState::Down)
		: name(inName)
		, type(inType)
		, state(inState) {}
	//
	inline bool Pressed(const std::string& inName) const { return (name == inName) && (state == LcKeyState::Down); }
	//
	inline bool Released(const std::string& inName) const { return (name == inName) && (state == LcKeyState::Up); }
	//
	std::string name;
	//
	LcActionType type;
	//
	LcKeyState state;
};

/** Key action */
struct LcKeyAction : public LcAction
{
	LcKeyAction(const std::string& inName, int inKey, LcKeyState inState)
		: LcAction(inName, LcActionType::Key, inState)
		, key(inKey) {}
	// LcKeys or LcJKeys
	int key;
};

/** Mouse action */
struct LcMouseAction : public LcAction
{
	LcMouseAction(const std::string& inName, int inButton, LcKeyState inState, float inX, float inY)
		: LcAction(inName, LcActionType::Mouse, inState)
		, button(inButton)
		, x(inX), y(inY) {}
	// LcMouseBtn
	int button;
	//
	float x;
	//
	float y;
};

/** Axis action */
struct LcAxisAction : public LcAction
{
	LcAxisAction(const std::string& inName, int inAxis, float inX, float inY)
		: LcAction(inName, LcActionType::Axis)
		, axis(inAxis)
		, x(inX), y(inY) {}
	// LcJAxis
	int axis;
	//
	float x;
	//
	float y;
};


/**
* Input device interface */
class IInputDevice
{
public:
	static const std::wstring MouseAndKeyboard;


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
	* Update device */
	virtual void Update(float deltaSeconds, const LcAppContext& context) = 0;
	/**
	* Get action state */
	virtual bool Pressed(const std::string& actionName) const = 0;
	/**
	* Get active state */
	virtual bool IsActive() const noexcept = 0;
	/**
	* Get joystick name */
	virtual std::wstring GetName() const = 0;
	/**
	* Get input state */
	virtual KEYS& GetState() noexcept = 0;
	/**
	* Get input state */
	virtual const KEYS& GetState() const noexcept = 0;
	/**
	* Get previous input state */
	virtual const KEYS& GetPrevState() const noexcept = 0;
	/**
	* Get device type */
	virtual LcInputDeviceType GetType() const noexcept = 0;
	/**
	* Get pointer position */
	virtual LcVector2 GetPointerPos() const noexcept { return LcVector2{}; }
	/**
	* Get previous pointer position */
	virtual LcVector2 GetPrevPointerPos() const noexcept { return LcVector2{}; }
	/**
	* Get pointer position */
	virtual void SetPointerPos(LcVector2 pos) noexcept {}
	/**
	* Get mutex */
	virtual std::mutex& GetMutex() = 0;

};


/** Input devices list */
typedef std::deque<std::unique_ptr<IInputDevice>> TInputDevicesList;

/** Keyboard events handler */
typedef std::function<void(int, LcKeyState, const LcAppContext&)> LcKeysHandler;

/** Mouse button handler */
typedef std::function<void(int, LcKeyState, float, float, const LcAppContext&)> LcMouseButtonHandler;

/** Mouse move handler */
typedef std::function<void(float, float, const LcAppContext&)> LcMouseMoveHandler;

/** Action handler */
typedef std::function<void(const LcAction& action, const LcAppContext&)> LcActionHandler;

/** Gamepad axis events handler */
typedef std::function<void(int, float, float, const LcAppContext&)> LcAxisHandler;


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
	virtual void Init(const LcAppContext& context) = 0;
	/**
	* Shutdown input system */
	virtual void Shutdown() = 0;
	/**
	* Update input system */
	virtual void Update(float deltaSeconds, const LcAppContext& context) = 0;
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
	* Set action handler */
	virtual void SetActionHandler(LcActionHandler handler) noexcept = 0;
	/**
	* Set mouse move handler */
	virtual void SetMouseMoveHandler(LcMouseMoveHandler handler) noexcept = 0;
	/**
	* Set mouse button handler */
	virtual void SetMouseButtonHandler(LcMouseButtonHandler handler) noexcept = 0;
	/**
	* Set gamepad axis handler */
	virtual void SetAxisHandler(LcAxisHandler handler) noexcept = 0;
	/**
	* Get keyboard handler */
	virtual LcKeysHandler& GetKeysHandler() noexcept = 0;
	/**
	* Get action handler */
	virtual LcActionHandler& GetActionHandler() noexcept = 0;
	/**
	* Get mouse move handler */
	virtual LcMouseMoveHandler& GetMouseMoveHandler() noexcept = 0;
	/**
	* Get mouse button handler */
	virtual LcMouseButtonHandler& GetMouseButtonHandler() noexcept = 0;
	/**
	* Get gamepad axis handler */
	virtual LcAxisHandler& GetAxisHandler() noexcept = 0;
	/**
	* Get input devices list */
	virtual const TInputDevicesList& GetInputDevicesList() const noexcept = 0;
	/**
	* Get input devices list */
	virtual TInputDevicesList& GetInputDevicesList() noexcept = 0;
	/**
	* Get active input device */
	virtual const IInputDevice* GetActiveInputDevice() const noexcept = 0;
	/**
	* Get active input device */
	virtual IInputDevice* GetActiveInputDevice() noexcept = 0;

};


/**
* Default input device with keys */
class CORE_API LcKeyboardInputDevice : public IInputDevice
{
public:
	static const std::wstring Name;


public:
	LcKeyboardInputDevice(const struct LcAppConfig* inCfg) : cfg(inCfg), name(Name), active(true) {}


public:
	//
	virtual ~LcKeyboardInputDevice() {}
	//
	virtual void Activate() override { active = true; }
	//
	virtual void Deactivate() override { active = false; }
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual bool Pressed(const std::string& actionName) const override;
	//
	virtual bool IsActive() const noexcept override { return active; }
	//
	virtual std::wstring GetName() const override { return name; }
	//
	virtual KEYS& GetState() noexcept override { return keys; }
	//
	virtual const KEYS& GetState() const noexcept override { return keys; }
	//
	virtual const KEYS& GetPrevState() const noexcept override { return prevKeys; }
	//
	virtual LcInputDeviceType GetType() const noexcept override { return LcInputDeviceType::Keyboard; }
	//
	virtual std::mutex& GetMutex() noexcept override { return mutex; }


protected:
	//
	KEYS keys;
	//
	KEYS prevKeys;
	//
	std::wstring name;
	//
	std::mutex mutex;
	//
	const LcAppConfig* cfg;
	//
	bool active;

};


/**
* Default input device with pointer */
class CORE_API LcMouseInputDevice : public IInputDevice
{
public:
	static const std::wstring Name;


public:
	LcMouseInputDevice(const LcAppConfig* inCfg) : cfg(inCfg), name(Name), mousePos{}, active(true) {}


public:
	//
	virtual ~LcMouseInputDevice() {}
	//
	virtual void Activate() override { active = true; }
	//
	virtual void Deactivate() override { active = false; }
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual bool Pressed(const std::string& actionName) const override;
	//
	virtual bool IsActive() const noexcept override { return active; }
	//
	virtual std::wstring GetName() const override { return name; }
	//
	virtual KEYS& GetState() noexcept override { return buttons; }
	//
	virtual const KEYS& GetState() const noexcept override { return buttons; }
	//
	virtual const KEYS& GetPrevState() const noexcept override { return prevButtons; }
	//
	virtual LcInputDeviceType GetType() const noexcept override { return LcInputDeviceType::Mouse; }
	//
	virtual LcVector2 GetPointerPos() const noexcept override { return mousePos; }
	//
	virtual LcVector2 GetPrevPointerPos() const noexcept override { return mousePos; }
	//
	virtual void SetPointerPos(LcVector2 pos) noexcept override { mousePos = pos; }
	//
	virtual std::mutex& GetMutex() override { return mutex; }


protected:
	//
	KEYS buttons;
	//
	KEYS prevButtons;
	//
	LcVector2 mousePos;
	//
	LcVector2 prevMousePos;
	//
	std::wstring name;
	//
	std::mutex mutex;
	//
	const struct LcAppConfig* cfg;
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


protected:
	//
	LcDefaultInputSystem(const LcDefaultInputSystem&) = delete;
	//
	LcDefaultInputSystem& operator=(const LcDefaultInputSystem&) = delete;


public: // IInputSystem interface implementation
	//
	virtual ~LcDefaultInputSystem() override {}
	//
	virtual void Init(const LcAppContext& context) override;
	//
	virtual void Shutdown() override {}
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual void SetActiveDevice(const std::wstring& deviceNamePart) override;
	//
	virtual void SetActiveDevice(const IInputDevice* device) override;
	//
	virtual void SetKeysHandler(LcKeysHandler handler) noexcept override { keysHandler = handler; }
	//
	virtual void SetActionHandler(LcActionHandler handler) noexcept override { actionHandler = handler; }
	//
	virtual void SetMouseMoveHandler(LcMouseMoveHandler handler) noexcept override { mouseMoveHandler = handler; }
	//
	virtual void SetMouseButtonHandler(LcMouseButtonHandler handler) noexcept override { mouseButtonHandler = handler; }
	//
	virtual void SetAxisHandler(LcAxisHandler handler) noexcept override { axisHandler = handler; }
	//
	virtual LcKeysHandler& GetKeysHandler() noexcept override { return keysHandler; }
	//
	virtual LcActionHandler& GetActionHandler() noexcept override { return actionHandler; }
	//
	virtual LcMouseMoveHandler& GetMouseMoveHandler() noexcept override { return mouseMoveHandler; }
	//
	virtual LcMouseButtonHandler& GetMouseButtonHandler() noexcept override { return mouseButtonHandler; }
	//
	virtual LcAxisHandler& GetAxisHandler() noexcept override { return axisHandler; }
	//
	virtual const TInputDevicesList& GetInputDevicesList() const noexcept override { return devices; }
	//
	virtual TInputDevicesList& GetInputDevicesList() noexcept override { return devices; }
	//
	virtual const IInputDevice* GetActiveInputDevice() const noexcept override { return activeDevice; }
	//
	virtual IInputDevice* GetActiveInputDevice() noexcept override { return activeDevice; }


protected:
	//
	LcKeysHandler keysHandler;
	//
	LcActionHandler actionHandler;
	//
	LcMouseMoveHandler mouseMoveHandler;
	//
	LcMouseButtonHandler mouseButtonHandler;
	//
	LcAxisHandler axisHandler;
	//
	IInputDevice* activeDevice;
	//
	TInputDevicesList devices;
	//
	struct LcAppConfig* cfg;

};


/**
* Default input system */
CORE_API TInputSystemPtr GetDefaultInputSystem();
