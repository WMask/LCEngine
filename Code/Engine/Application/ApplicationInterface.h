/**
* ApplicationInterface.h
* 10.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "GUI/Module.h"
#include "World/Module.h"
#include "RenderSystem/Module.h"
#include "Application/AppConfig.h"
#include "Core/InputSystem.h"
#include "Core/LCTypes.h"

#include <string>
#include <memory>
#include <functional>

#pragma warning(disable : 4251)


/** Init handler */
typedef std::function<void(struct LcAppContext&)> LcInitHandler;

/** Update handler */
typedef std::function<void(float, struct LcAppContext&)> LcUpdateHandler;


/** Application stats */
struct LcAppStats
{
	int numSprites;
	int numWidgets;
	int numTextures;
	int numTilemaps;
	int numFonts;
	int numSounds;
	int numBodies;
};


/**
* Application interface */
class IApplication
{
public:
	//
	IApplication() {}
	//
	IApplication(const IApplication&) = delete;
	//
	IApplication& operator=(const IApplication&) = delete;


public:
	/**
	* Virtual destructor */
	virtual ~IApplication() {}
	/**
	* Set app parameters */
	virtual void Init(void* handle, const std::wstring& cmds, int cmdsCount, const char* shadersPath = nullptr) noexcept = 0;
	/**
	* Set app parameters */
	virtual void Init(void* handle, const std::wstring& cmds, const char* shadersPath = nullptr) noexcept = 0;
	/**
	* Set app parameters */
	virtual void Init(void* handle) noexcept = 0;
	/**
	* Set render system */
	virtual void SetRenderSystem(TRenderSystemPtr render) noexcept = 0;
	/**
	* Set script system */
	virtual void SetScriptSystem(TScriptSystemPtr scripts) noexcept = 0;
	/**
	* Set audio system */
	virtual void SetAudioSystem(TAudioSystemPtr audio) noexcept = 0;
	/**
	* Set input system */
	virtual void SetInputSystem(TInputSystemPtr audio) noexcept = 0;
	/**
	* Set physics world */
	virtual void SetPhysicsWorld(TPhysicsWorldPtr physWorld) noexcept = 0;
	/**
	* Set GUI manager */
	virtual void SetGuiManager(TGuiManagerPtr gui) noexcept = 0;
	/**
	* Set window size in pixels */
	virtual void SetWindowSize(unsigned int width, unsigned int height) = 0;
	/**
	* Set window mode */
	virtual void SetWindowMode(LcWinMode mode) = 0;
	/**
	* Set vertical synchronization mode */
	virtual void SetVSync(bool inVSync) noexcept = 0;
	/**
	* Set fullscreen mode type.
	* If true: actually changes monitor resolution. If VSync true: FPS - equal to monitor highest refresh rate.
	* If false: fake windowed fullscreen mode. If VSync true: FPS - default OS refresh rate. */
	virtual void SetAllowFullscreen(bool inAllowFullscreen) noexcept = 0;
	/**
	* @brief Set No Delay mode.
	* If true: high FPS and update rate.
	* If true and VSync false: processor core utilization 100%, highest FPS and update rate.  */
	virtual void SetNoDelay(bool inNoDelay) noexcept = 0;
	/**
	* Set init handler */
	virtual void SetInitHandler(LcInitHandler handler) noexcept = 0;
	/**
	* Set update handler */
	virtual void SetUpdateHandler(LcUpdateHandler handler) noexcept = 0;
	/**
	* Set application config */
	virtual void SetConfig(const LcAppConfig& cfg) noexcept = 0;
	/**
	* Run application main loop */
	virtual void Run() = 0;
	/**
	* Remove all sprites, widgets, textures etc. */
	virtual void ClearWorld(bool removeRooted = false) = 0;
	/**
	* Request application quit */
	virtual void RequestQuit() noexcept = 0;
	/**
	* Get window width in pixels */
	virtual int GetWindowWidth() const = 0;
	/**
	* Get window height in pixels */
	virtual int GetWindowHeight() const = 0;
	/**
	* Get vertical synchronization mode */
	virtual bool GetVSync() const noexcept = 0;
	/**
	* Get application config */
	virtual LcAppConfig& GetConfig() noexcept = 0;
	/**
	* Get application config */
	virtual const LcAppConfig& GetConfig() const noexcept = 0;
	/**
	* Get application context */
	virtual const LcAppContext& GetContext() const noexcept = 0;
	/**
	* Get application stats */
	virtual LcAppStats GetAppStats() const noexcept = 0;
	/**
	* Get Input system pointer */
	virtual class IInputSystem* GetInputSystem() noexcept = 0;
	/**
	* Get Input system pointer */
	virtual TInputSystemPtr GetInputSystemPtr() noexcept = 0;

};
