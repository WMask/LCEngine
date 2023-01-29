/**
* WindowsApplication.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <windows.h>

#include "WindowsApplicationModule.h"
#include "Application/Application.h"


/**
* Win32 Application class */
class WINDOWSAPPLICATION_API LCWindowsApplication : public IApplication
{
public:
	/**
	* Default constructor */
	LCWindowsApplication();


public: // IApplication interface implementation
	/**
	* Virtual destructor */
	virtual ~LCWindowsApplication() override;
	/**
	* Set app parameters */
	virtual void Init(void* Handle, LCSTR* cmds, int cmdsCount) noexcept override;
	/**
	* Set app parameters */
	virtual void Init(void* Handle, LCSTR cmds) noexcept override;
	/**
	* Set app parameters */
	virtual void SetRenderSystemType(ERenderSystemType inType) noexcept { type = inType; }
	/**
	* Set window size in pixels */
	virtual void SetWindowSize(LCSize inWindowSize) noexcept { windowSize = inWindowSize; }
	/**
	* Set update handler */
	virtual void SetUpdateHandler(UpdateHandler handler) noexcept { updateHandler = handler; }
	/**
	* Set keyboard handler */
	virtual void SetKeyboardHandler(KeyboardHandler handler) noexcept { keyboardHandler = handler; }
	/**
	* Set mouse handler */
	virtual void SetMouseHandler(MouseHandler handler) noexcept { mouseHandler = handler; }
	/**
	* Run application main loop */
	virtual void Run() override;
	/**
	* Request application quit */
	virtual void RequestQuit() noexcept { quit = true; }


protected:
	/**
	* Update application */
	void OnUpdate();


protected:
	HINSTANCE hInstance;
	//
	HWND hWnd;
	//
	std::shared_ptr<class IRenderSystem> renderSystem;
	//
	ERenderSystemType type;
	//
	LCSTR* cmds;
	//
	int cmdsCount;
	//
	LCSize windowSize;
	//
	bool quit;
	//
	UpdateHandler updateHandler;
	//
	KeyboardHandler keyboardHandler;
	//
	MouseHandler mouseHandler;
	//
	ULONGLONG prevTick;

};
