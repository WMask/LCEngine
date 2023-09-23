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
#include "Core/InputSystem.h"
#include "Core/LCTypes.h"

#include <string>
#include <memory>
#include <functional>

#pragma warning(disable : 4251)


/** Init handler */
typedef std::function<void(class IApplication*)> LcInitHandler;

/** Update handler */
typedef std::function<void(float, class IApplication*)> LcUpdateHandler;


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
	virtual void SetWindowSize(int width, int height) = 0;
	/**
	* Set window mode */
	virtual void SetWindowMode(LcWinMode mode) = 0;
	/**
	* Set vertical synchronization mode */
	virtual void SetVSync(bool inVSync) noexcept = 0;
	/**
	* Set init handler */
	virtual void SetInitHandler(LcInitHandler handler) noexcept = 0;
	/**
	* Set update handler */
	virtual void SetUpdateHandler(LcUpdateHandler handler) noexcept = 0;
	/**
	* Run application main loop */
	virtual void Run() = 0;
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
	* Get World pointer */
	virtual class IWorld* GetWorld() noexcept = 0;
	/**
	* Get World pointer */
	virtual TWorldPtr GetWorldPtr() noexcept = 0;
	/**
	* Get Script system pointer */
	virtual class IScriptSystem* GetScriptSystem() noexcept = 0;
	/**
	* Get Script system pointer */
	virtual TScriptSystemPtr GetScriptSystemPtr() noexcept = 0;
	/**
	* Get Audio system pointer */
	virtual class IAudioSystem* GetAudioSystem() noexcept = 0;
	/**
	* Get Audio system pointer */
	virtual TAudioSystemPtr GetAudioSystemPtr() noexcept = 0;
	/**
	* Get Input system pointer */
	virtual class IInputSystem* GetInputSystem() noexcept = 0;
	/**
	* Get Input system pointer */
	virtual TInputSystemPtr GetInputSystemPtr() noexcept = 0;
	/**
	* Get Physics World pointer */
	virtual class IPhysicsWorld* GetPhysicsWorld() noexcept = 0;
	/**
	* Get Physics World pointer */
	virtual TPhysicsWorldPtr GetPhysicsWorldPtr() noexcept = 0;

};
