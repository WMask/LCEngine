/**
* WindowsApplication.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <windows.h>

#include "Module.h"
#include "Application/Application.h"
#include "Core/LCTypesEx.h"


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
	virtual void Init(void* handle, TWeakWorld worldPtr, const std::wstring& cmds, int cmdsCount, const char* shadersPath) noexcept override;
	/**
	* Set app parameters */
	virtual void Init(void* handle, TWeakWorld worldPtr, const std::wstring& cmds, const char* shadersPath) noexcept override;
	/**
	* Set app parameters */
	virtual void Init(void* handle, TWeakWorld worldPtr = TWeakWorld()) noexcept override;
	/**
	* Set render system */
	virtual void SetRenderSystem(TRenderSystemPtr render) noexcept { renderSystem = render; }
	/**
	* Set GUI manager */
	virtual void SetGuiManager(TGuiManagerPtr gui) noexcept { guiManager = gui; }
	/**
	* Set window size in pixels */
	virtual void SetWindowSize(int width, int height) noexcept { windowSize = LcSize(width, height); }
	/**
	* Set init handler */
	virtual void SetInitHandler(LcInitHandler handler) noexcept { initHandler = handler; }
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
	/**
	* Get World */
	virtual class IWorld* GetWorld() noexcept;
	/**
	* Get World pointer */
	virtual TWeakWorld GetWorldPtr() noexcept { return world; }


protected:
	void OnUpdate();


protected:
	HINSTANCE hInstance;
	//
	HWND hWnd;
	//
	TWeakWorld world;
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
	LcInitHandler initHandler;
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
