/**
* RenderSystem.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "GUI/Module.h"
#include "World/Module.h"
#include "Core/LcTypesEx.h"

#include <map>
#include <string>

#pragma warning(disable : 4251)

typedef std::shared_ptr<class IWidgetRender> TWidgetRenderPtr;
typedef std::weak_ptr<class IWidgetRender> TWeakWidgetRender;


/**
* Render system interface */
class RENDERSYSTEM_API IRenderSystem
{
public:
	/**
	* Virtual destructor */
	virtual ~IRenderSystem() {}
	/**
	* Load shaders */
	virtual void LoadShaders(const char* folderPath) = 0;
	/**
	* Create render system */
	virtual void Create(TWeakWorld worldPtr, void* windowHandle, bool windowed) = 0;
	/**
	* Set GUI manager */
	virtual void SetGuiManager(TWeakGuiManager guiManager) = 0;
	/**
	* Shutdown render system */
	virtual void Shutdown() = 0;
	/**
	* Update world */
	virtual void Update(float deltaSeconds) = 0;
	/**
	* Render world */
	virtual void Render() = 0;
	/**
	* Return render system state */
	virtual bool CanRender() const = 0;
	/**
	* Update camera */
	virtual void UpdateCamera(float deltaSeconds, LcVector3 newPos, LcVector3 newTarget) = 0;
	/**
	* Get widget render */
	virtual TWidgetRenderPtr GetWidgetRender() const = 0;
	/**
	* Return render system type */
	virtual LcRenderSystemType GetType() const = 0;

};


/**
* Render system interface */
class RENDERSYSTEM_API LcRenderSystemBase : public IRenderSystem
{
public:
	typedef std::map<std::string, std::string> SHADERS_MAP;
	//
	LcRenderSystemBase() : cameraPos(LcDefaults::ZeroVec3), cameraTarget(LcDefaults::ZeroVec3) {}


public:// IRenderSystem interface implementation
	/**
	* Load shaders */
	virtual void LoadShaders(const char* folderPath) override;
	/**
	* Create render system */
	virtual void Create(TWeakWorld worldPtr, void* windowHandle, bool windowed) override { world = worldPtr; }
	/**
	* Set GUI manager */
	virtual void SetGuiManager(TWeakGuiManager inGuiManager) override { guiManager = inGuiManager; }
	/**
	* Shutdown render system */
	virtual void Shutdown() override {}
	/**
	* Update world */
	virtual void Update(float deltaSeconds) override;
	/**
	* Render world */
	virtual void Render() override;


protected:
	/**
	* Render sprite */
	virtual void RenderSprite(const class ISprite* sprite) = 0;
	/**
	* Pre render widget */
	virtual void PreRenderWidget() = 0;
	/**
	* Pre render widget */
	virtual void PostRenderWidget() = 0;


protected:
	TWeakWorld world;
	//
	TWeakGuiManager guiManager;
	//
	SHADERS_MAP shaders;
	//
	LcVector3 cameraPos;
	//
	LcVector3 cameraTarget;

};
