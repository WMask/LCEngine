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
	//
	virtual ~LcWindowsApplication() override;
	//
	virtual void Init(void* handle, const std::wstring& cmds, int cmdsCount, const char* shadersPath) noexcept override;
	//
	virtual void Init(void* handle, const std::wstring& cmds, const char* shadersPath) noexcept override;
	//
	virtual void Init(void* handle) noexcept override;
	//
	virtual void SetRenderSystem(TRenderSystemPtr render) noexcept { renderSystem = render; }
	//
	virtual void SetGuiManager(TGuiManagerPtr gui) noexcept { guiManager = gui; }
	//
	virtual void SetWindowSize(int width, int height) override;
	//
	virtual void SetWindowMode(LcWinMode mode) override;
	//
	virtual void SetInitHandler(LcInitHandler handler) noexcept { initHandler = handler; }
	//
	virtual void SetUpdateHandler(LcUpdateHandler handler) noexcept { updateHandler = handler; }
	//
	virtual void SetKeyboardHandler(LcKeyboardHandler handler) noexcept { keyboardHandler = handler; }
	//
	virtual void SetMouseMoveHandler(LcMouseMoveHandler handler) noexcept { mouseMoveHandler = handler; }
	//
	virtual void SetMouseButtonHandler(LcMouseButtonHandler handler) noexcept { mouseButtonHandler = handler; }
	//
	virtual void Run() override;
	//
	virtual void RequestQuit() noexcept override { quit = true; }
	//
	virtual int GetWindowWidth() const override { return windowSize.x; }
	//
	virtual int GetWindowHeight() const override { return windowSize.y; }
	//
	virtual class IWorld* GetWorld() noexcept override { return world.get(); }
	//
	virtual TWorldPtr GetWorldPtr() noexcept override { return world; }


protected:
	void OnUpdate();


protected:
	HINSTANCE hInstance;
	//
	HWND hWnd;
	//
	TWorldPtr world;
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
	LcWinMode winMode;
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
