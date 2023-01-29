/**
* Application.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <memory>
#include <functional>

#include "ApplicationModule.h"
#include "RenderSystem/RenderSystem.h"
#include "Core/LCTypes.h"


#pragma warning(disable : 4251)

/**
* Update handler */
typedef std::function<void(float)> UpdateHandler;

/**
* Input key event type */
enum class EInputKeyEvent
{
	Down,
	Up
};

/**
* Keyboard events handler */
typedef std::function<void(int, EInputKeyEvent)> KeyboardHandler;

/**
* Mouse events handler */
typedef std::function<void(int, EInputKeyEvent, float, float)> MouseHandler;


/**
* Application interface */
class APPLICATION_API IApplication
{
public:
	/**
	* Returns platform-specific application */
	static std::shared_ptr<IApplication> GetPlatformApp();


public:
	/**
	* Virtual destructor */
	virtual ~IApplication();
	/**
	* Set app parameters */
	virtual void Init(void* Handle, LCSTR* cmds, int cmdsCount) noexcept = 0;
	/**
	* Set app parameters */
	virtual void Init(void* Handle, LCSTR cmds) noexcept = 0;
	/**
	* Set render system type */
	virtual void SetRenderSystemType(ERenderSystemType type) noexcept = 0;
	/**
	* Set window size in pixels */
	virtual void SetWindowSize(LCSize windowSize) noexcept = 0;
	/**
	* Set update handler */
	virtual void SetUpdateHandler(UpdateHandler handler) noexcept = 0;
	/**
	* Set keyboard handler */
	virtual void SetKeyboardHandler(KeyboardHandler handler) noexcept = 0;
	/**
	* Set mouse handler */
	virtual void SetMouseHandler(MouseHandler handler) noexcept = 0;
	/**
	* Run application main loop */
	virtual void Run() = 0;
	/**
	* Request application quit */
	virtual void RequestQuit() noexcept = 0;


protected:
	IApplication();
	//
	IApplication(const IApplication&);
	//
	IApplication& operator=(const IApplication&);

};

/**
* Weak pointer for handlers */
typedef std::weak_ptr<IApplication> TWeakApp;
