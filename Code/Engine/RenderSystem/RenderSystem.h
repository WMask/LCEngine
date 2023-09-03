/**
* RenderSystem.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "GUI/Module.h"
#include "World/Visual.h"
#include "Core/LcTypesEx.h"

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
	virtual void Create(TWeakWorld worldPtr, void* windowHandle, LcWinMode mode) = 0;
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
	* Request resize */
	virtual void RequestResize(int width, int height) = 0;
	/**
	* Resize render system */
	virtual void Resize(int width, int height) = 0;
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


/** Widget render mode */
enum EWRMode { Textures, Text };

/**
* Render system interface */
class RENDERSYSTEM_API LcRenderSystemBase : public IRenderSystem
{
public:
	typedef std::map<std::string, std::string> SHADERS_MAP;
	//
	LcRenderSystemBase() : cameraPos(LcDefaults::ZeroVec3), cameraTarget(LcDefaults::ZeroVec3) {}


public:// IRenderSystem interface implementation
	//
	virtual void LoadShaders(const char* folderPath) override;
	//
	virtual void Create(TWeakWorld world, void* windowHandle, LcWinMode mode) override { worldPtr = world; }
	//
	virtual void Shutdown() override {}
	//
	virtual void Update(float deltaSeconds) override;
	//
	virtual void Render() override;
	//
	virtual void RequestResize(int width, int height) override {}
	//
	virtual void Resize(int width, int height) override {}
	//
	virtual void SetMode(LcWinMode mode) override {}


protected:
	/**
	* Render sprite */
	virtual void RenderSprite(const class ISprite* sprite) = 0;
	/**
	* Render widget */
	virtual void RenderWidget(const class IWidget* widget) = 0;
	/**
	* Pre render widgets */
	virtual void PreRenderWidgets(EWRMode mode) = 0;
	/**
	* Post render widgets */
	virtual void PostRenderWidgets(EWRMode mode) = 0;


protected:
	TWeakWorld worldPtr;
	//
	SHADERS_MAP shaders;
	//
	LcVector3 cameraPos;
	//
	LcVector3 cameraTarget;

};


/**
* 2D Visual renderer interface */
class IVisual2DRender
{
public:
	/**
	* Setup render state */
	virtual void Setup() = 0;
	/**
	* Add font */
	virtual const struct ITextFont* AddFont(const std::wstring& fontName, unsigned short fontSize, LcFontWeight fontWeight = LcFontWeight::Normal) = 0;
	/**
	* Render sprite */
	virtual void RenderSprite(const class ISprite* sprite) = 0;
	/**
	* Render widget */
	virtual void RenderWidget(const class IWidget* widget) = 0;
	/**
	* Checks support for the feature */
	virtual bool Supports(const TVFeaturesList& features) const = 0;

};
