/**
* Application.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <memory>
#include <functional>

#include "Module.h"
#include "Core/LCTypes.h"

#pragma warning(disable : 4251)


/**
* Update handler */
typedef std::function<void(float)> LcUpdateHandler;

/**
* Keyboard events handler */
typedef std::function<void(int, LcKeyState)> LcKeyboardHandler;

/**
* Mouse move handler */
typedef std::function<void(float, float)> LcMouseMoveHandler;

/**
* Mouse button handler */
typedef std::function<void(LcMouseBtn, LcKeyState, float, float)> LcMouseButtonHandler;


/**
* Application interface */
class APPLICATION_API IApplication
{
public:
	/**
	* Virtual destructor */
	virtual ~IApplication();
	/**
	* Set app parameters */
	virtual void Init(void* Handle, const std::wstring& cmds, int cmdsCount) noexcept = 0;
	/**
	* Set app parameters */
	virtual void Init(void* Handle, const std::wstring& cmds) noexcept = 0;
	/**
	* Set render system */
	virtual void SetRenderSystem(std::shared_ptr<class IRenderSystem> render) noexcept = 0;
	/**
	* Set GUI manager */
	virtual void SetGuiManager(std::shared_ptr<class IGuiManager> manager) noexcept = 0;
	/**
	* Set window size in pixels */
	virtual void SetWindowSize(LcSize windowSize) noexcept = 0;
	/**
	* Set update handler */
	virtual void SetUpdateHandler(LcUpdateHandler handler) noexcept = 0;
	/**
	* Set keyboard handler */
	virtual void SetKeyboardHandler(LcKeyboardHandler handler) noexcept = 0;
	/**
	* Set mouse move handler */
	virtual void SetMouseMoveHandler(LcMouseMoveHandler handler) noexcept = 0;
	/**
	* Set mouse button handler */
	virtual void SetMouseButtonHandler(LcMouseButtonHandler handler) noexcept = 0;
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
