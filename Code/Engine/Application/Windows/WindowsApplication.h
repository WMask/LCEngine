/**
* WindowsApplication.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <windows.h>

#include "Module.h"
#include "Application/ApplicationInterface.h"
#include "Core/LCTypesEx.h"

#pragma warning(disable : 4275)


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
	virtual void SetRenderSystem(TRenderSystemPtr render) noexcept override { renderSystem = render; }
	//
	virtual void SetScriptSystem(TScriptSystemPtr scripts) noexcept override { scriptSystem = scripts; }
	//
	virtual void SetAudioSystem(TAudioSystemPtr audio) noexcept override { audioSystem = audio; }
	//
	virtual void SetInputSystem(TInputSystemPtr input) noexcept override { if (input) inputSystem = input; }
	//
	virtual void SetPhysicsWorld(TPhysicsWorldPtr inPhysWorld) noexcept override { physWorld = inPhysWorld; }
	//
	virtual void SetGuiManager(TGuiManagerPtr gui) noexcept override { guiManager = gui; }
	//
	virtual void SetWindowSize(int width, int height) override;
	//
	virtual void SetWindowMode(LcWinMode mode) override;
	//
	virtual void SetVSync(bool inVSync) noexcept override { vSync = inVSync; }
	//
	virtual void SetAllowFullscreen(bool inAllowFullscreen) noexcept override { allowFullscreen = inAllowFullscreen; }
	//
	virtual void SetNoDelay(bool inNoDelay) noexcept override { noDelay = inNoDelay; }
	//
	virtual void SetInitHandler(LcInitHandler handler) noexcept override { initHandler = handler; }
	//
	virtual void SetUpdateHandler(LcUpdateHandler handler) noexcept override { updateHandler = handler; }
	//
	virtual void Run() override;
	//
	virtual void ClearWorld() override;
	//
	virtual void RequestQuit() noexcept override { quit = true; }
	//
	virtual int GetWindowWidth() const override { return windowSize.x; }
	//
	virtual int GetWindowHeight() const override { return windowSize.y; }
	//
	virtual bool GetVSync() const noexcept override { return vSync; }
	//
	virtual const LcAppContext& GetContext() const noexcept { return context; }
	//
	virtual LcAppStats GetAppStats() const noexcept;
	//
	virtual class IInputSystem* GetInputSystem() noexcept override { return inputSystem.get(); }
	//
	virtual TInputSystemPtr GetInputSystemPtr() noexcept override { return inputSystem; }


protected:
	void OnUpdate();


protected:
	HINSTANCE hInstance;
	//
	HWND hWnd;
	//
	std::wstring cmds;
	//
	int cmdsCount;
	//
	bool quit;
	//
	bool vSync;
	//
	bool allowFullscreen;
	//
	bool noDelay;
	//
	LcSize windowSize;
	//
	LcWinMode winMode;
	//
	LARGE_INTEGER prevTime;
	//
	LARGE_INTEGER frequency;
	//
	std::string shadersPath;


protected:
	TWorldPtr world;
	//
	TRenderSystemPtr renderSystem;
	//
	TScriptSystemPtr scriptSystem;
	//
	TAudioSystemPtr audioSystem;
	//
	TInputSystemPtr inputSystem;
	//
	TPhysicsWorldPtr physWorld;
	//
	TGuiManagerPtr guiManager;
	//
	LcAppContext context;


protected:
	LcInitHandler initHandler;
	//
	LcUpdateHandler updateHandler;

};
