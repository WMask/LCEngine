/**
* WindowsApplication.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <windows.h>

#include "Module.h"
#include "Application/Application.h"


/**
* Win32 Application class */
class WINDOWSAPPLICATION_API LcWindowsApplication : public IApplication
{
public:
	/**
	* Default constructor */
	LcWindowsApplication();


public: // IApplication interface implementation
	/**
	* Virtual destructor */
	virtual ~LcWindowsApplication() override;
	/**
	* Set app parameters */
	virtual void Init(void* handle, TWorldWeakPtr worldPtr, const std::wstring& cmds, int cmdsCount, const char* shadersPath) noexcept override;
	/**
	* Set app parameters */
	virtual void Init(void* handle, TWorldWeakPtr worldPtr, const std::wstring& cmds, const char* shadersPath) noexcept override;
	/**
	* Set app parameters */
	virtual void Init(void* handle, TWorldWeakPtr worldPtr = TWorldWeakPtr()) noexcept override;
	/**
	* Set render system */
	virtual void SetRenderSystem(TRenderSystemPtr render) noexcept { renderSystem = std::move(render); }
	/**
	* Set GUI manager */
	virtual void SetGuiManager(TGuiManagerPtr gui) noexcept { guiManager = std::move(gui); }
	/**
	* Set window size in pixels */
	virtual void SetWindowSize(LcSize inWindowSize) noexcept { windowSize = inWindowSize; }
	/**
	* Set update handler */
	virtual void SetUpdateHandler(LcUpdateHandler handler) noexcept { updateHandler = handler; }
	/**
	* Set keyboard handler */
	virtual void SetKeyboardHandler(LcKeyboardHandler handler) noexcept { keyboardHandler = handler; }
	/**
	* Set mouse move handler */
	virtual void SetMouseMoveHandler(LcMouseMoveHandler handler) noexcept { mouseMoveHandler = handler; }
	/**
	* Set mouse button handler */
	virtual void SetMouseButtonHandler(LcMouseButtonHandler handler) noexcept { mouseButtonHandler = handler; }
	/**
	* Run application main loop */
	virtual void Run() override;
	/**
	* Request application quit */
	virtual void RequestQuit() noexcept { quit = true; }


protected:
	void OnUpdate();


protected:
	HINSTANCE hInstance;
	//
	HWND hWnd;
	//
	TWorldWeakPtr world;
	//
	TRenderSystemPtr renderSystem;
	//
	TGuiManagerPtr guiManager;
	//
	std::wstring cmds;
	//
	int cmdsCount;
	//
	std::string shadersPath;
	//
	LcSize windowSize;
	//
	bool quit;
	//
	LcUpdateHandler updateHandler;
	//
	LcKeyboardHandler keyboardHandler;
	//
	LcMouseMoveHandler mouseMoveHandler;
	//
	LcMouseButtonHandler mouseButtonHandler;
	//
	ULONGLONG prevTick;

};
