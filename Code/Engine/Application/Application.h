/**
* Application.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <memory>
#include <functional>
#include <map>

#include "ApplicationModule.h"
#include "RenderSystem/RenderSystem.h"
#include "Core/LCTypes.h"


#pragma warning(disable : 4251)

/**
* Update handler */
typedef std::function<void(float)> UpdateHandler;

/**
* Input key event type */
enum class EInputKeyEvent
{
	Down,
	Up
};

/**
* Keyboard events handler */
typedef std::function<void(int, EInputKeyEvent)> KeyboardHandler;

/**
* Mouse events handler */
typedef std::function<void(int, EInputKeyEvent, float, float)> MouseHandler;


/**
* Application interface */
class APPLICATION_API IApplication
{
public:
	/**
	* Returns platform-specific application */
	static std::shared_ptr<IApplication> GetPlatformApp();
	//
	typedef std::map<std::string, std::string> SHADERS_MAP;


public:
	/**
	* Virtual destructor */
	virtual ~IApplication();
	/**
	* Set app parameters */
	virtual void Init(void* Handle, LCSTR* cmds, int cmdsCount) noexcept = 0;
	/**
	* Set app parameters */
	virtual void Init(void* Handle, LCSTR cmds) noexcept = 0;
	/**
	* Load shaders */
	virtual void LoadShaders(const std::string& folderPath) = 0;
	/**
	* Get shaders */
	virtual const SHADERS_MAP& GetShaders() const noexcept = 0;
	/**
	* Set render system type */
	virtual void SetRenderSystemType(ERenderSystemType type) noexcept = 0;
	/**
	* Set window size in pixels */
	virtual void SetWindowSize(LCSize windowSize) noexcept = 0;
	/**
	* Set update handler */
	virtual void SetUpdateHandler(UpdateHandler handler) noexcept = 0;
	/**
	* Set keyboard handler */
	virtual void SetKeyboardHandler(KeyboardHandler handler) noexcept = 0;
	/**
	* Set mouse handler */
	virtual void SetMouseHandler(MouseHandler handler) noexcept = 0;
	/**
	* Set NoesisGUI flag */
	virtual void SetUseNoesis(bool useNoesis) noexcept = 0;
	/**
	* Get NoesisGUI flag */
	virtual bool GetUseNoesis() const noexcept = 0;
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

/**
* Weak pointer for handlers */
typedef std::weak_ptr<IApplication> TWeakApp;
