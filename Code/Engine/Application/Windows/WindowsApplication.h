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
	virtual void Init(void* Handle, const std::wstring& cmds, int cmdsCount) noexcept override;
	/**
	* Set app parameters */
	virtual void Init(void* Handle, const std::wstring& cmds) noexcept override;
	/**
	* Set render system */
	virtual void SetRenderSystem(std::shared_ptr<class IRenderSystem> render) noexcept { renderSystem = std::shared_ptr<class IRenderSystem>(render); }
	/**
	* Set GUI manager */
	virtual void SetGuiManager(std::shared_ptr<class IGuiManager> manager) noexcept { guiManager = std::shared_ptr<class IGuiManager>(manager); }
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
	std::shared_ptr<class IRenderSystem> renderSystem;
	//
	std::shared_ptr<class IGuiManager> guiManager;
	//
	std::wstring cmds;
	//
	int cmdsCount;
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
