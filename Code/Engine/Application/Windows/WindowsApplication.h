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
	* Load shaders */
	virtual void LoadShaders(const std::string& folderPath) override;
	/**
	* Get shaders */
	virtual const SHADERS_MAP& GetShaders() const noexcept override { return shaders; }
	/**
	* Set app parameters */
	virtual void SetRenderSystemType(LcRenderSystemType inType) noexcept { type = inType; }
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
	* Set mouse handler */
	virtual void SetMouseHandler(LcMouseHandler handler) noexcept { mouseHandler = handler; }
	/**
	* Set NoesisGUI flag */
	virtual void SetUseNoesis(bool inUseNoesis) noexcept { useNoesis = inUseNoesis; }
	/**
	* Get NoesisGUI flag */
	virtual bool GetUseNoesis() const noexcept { return useNoesis; }
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
	LcRenderSystemType type;
	//
	SHADERS_MAP shaders;
	//
	std::wstring cmds;
	//
	int cmdsCount;
	//
	LcSize windowSize;
	//
	bool quit;
	//
	bool useNoesis;
	//
	LcUpdateHandler updateHandler;
	//
	LcKeyboardHandler keyboardHandler;
	//
	LcMouseHandler mouseHandler;
	//
	ULONGLONG prevTick;

};
