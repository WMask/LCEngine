/**
* Application.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <string>
#include <memory>
#include <functional>

#include "Module.h"
#include "GUI/Module.h"
#include "World/Module.h"
#include "RenderSystem/Module.h"
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
	virtual void Init(void* handle, TWorldWeakPtr worldPtr, const std::wstring& cmds, int cmdsCount, const char* shadersPath = nullptr) noexcept = 0;
	/**
	* Set app parameters */
	virtual void Init(void* handle, TWorldWeakPtr worldPtr, const std::wstring& cmds, const char* shadersPath = nullptr) noexcept = 0;
	/**
	* Set app parameters */
	virtual void Init(void* handle, TWorldWeakPtr worldPtr = TWorldWeakPtr()) noexcept = 0;
	/**
	* Set render system */
	virtual void SetRenderSystem(TRenderSystemPtr render) noexcept = 0;
	/**
	* Set GUI manager */
	virtual void SetGuiManager(TGuiManagerPtr gui) noexcept = 0;
	/**
	* Set window size in pixels */
	virtual void SetWindowSize(int width, int height) noexcept = 0;
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
