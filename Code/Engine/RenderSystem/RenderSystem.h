/**
* RenderSystem.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "GUI/Module.h"
#include "World/Visual.h"
#include "Core/LCTypesEx.h"
#include "Core/LCDelegate.h"

#include <map>
#include <string>

#pragma warning(disable : 4251)


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
	virtual void Create(void* windowHandle, LcWinMode mode, bool vSync, const LcAppContext& context) = 0;
	/**
	* Shutdown render system */
	virtual void Shutdown() = 0;
	/**
	* Subscribe to world scale delegate */
	virtual void Subscribe(const LcAppContext& context) = 0;
	/**
	* Update world */
	virtual void Update(float deltaSeconds, const LcAppContext& context) = 0;
	/**
	* Render world */
	virtual void Render(const LcAppContext& context) = 0;
	/**
	* Return render system state */
	virtual bool CanRender() const = 0;
	/**
	* Request resize */
	virtual void RequestResize(int width, int height) = 0;
	/**
	* Resize render system */
	virtual void Resize(int width, int height, const LcAppContext& context) = 0;
	/**
	* Set window mode */
	virtual void SetMode(LcWinMode mode) = 0;
	/**
	* Update camera */
	virtual void UpdateCamera(float deltaSeconds, LcVector3 newPos, LcVector3 newTarget) = 0;
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
	LcRenderSystemBase() : cameraPos(LcDefaults::ZeroVec3), cameraTarget(LcDefaults::ZeroVec3), vSync(false) {}


public:// IRenderSystem interface implementation
	//
	virtual void LoadShaders(const char* folderPath) override;
	//
	virtual void Create(void* windowHandle, LcWinMode mode, bool vSync, const LcAppContext& context) override;
	//
	virtual void Shutdown() override {}
	//
	virtual void Subscribe(const LcAppContext& context) {}
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual void Render(const LcAppContext& context) override;
	//
	virtual void RequestResize(int width, int height) override {}
	//
	virtual void Resize(int width, int height, const LcAppContext& context) override {}
	//
	virtual void SetMode(LcWinMode mode) override {}


protected:
	/**
	* Render sprite */
	virtual void RenderSprite(const class ISprite* sprite, const LcAppContext& context) = 0;
	/**
	* Render widget */
	virtual void RenderWidget(const class IWidget* widget, const LcAppContext& context) = 0;


protected:
	SHADERS_MAP shaders;
	//
	LcVector3 cameraPos;
	//
	LcVector3 cameraTarget;
	//
	bool vSync;

};


/**
* 2D Visual renderer interface */
class IVisual2DRender
{
public:
	/**
	* Setup render state */
	virtual void Setup(const IVisual* visual, const LcAppContext& context) = 0;
	/**
	* Render sprite */
	virtual void RenderSprite(const class ISprite* sprite, const LcAppContext& context) = 0;
	/**
	* Render widget */
	virtual void RenderWidget(const class IWidget* widget, const LcAppContext& context) = 0;
	/**
	* Checks support for the feature */
	virtual bool Supports(const TVFeaturesList& features) const = 0;

};
