/**
* RenderSystem.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "RenderSystemModule.h"
#include "World/World.h"


/**
* Render system type */
enum class LcRenderSystemType
{
	Null,
	DX7,
	DX9,
	DX10
};


/**
* Render system interface */
class RENDERSYSTEM_API IRenderSystem
{
public:
	IRenderSystem(class IApplication& app);


public:
	/**
	* Virtual destructor */
	virtual ~IRenderSystem();
	/**
	* Create render system */
	virtual void Create(void* Handle, LcSize viewportSize, bool windowed);
	/**
	* Shutdown render system */
	virtual void Shutdown();
	/**
	* Update world */
	virtual void Update(float deltaSeconds) = 0;
	/**
	* Render world */
	virtual void Render();
	/**
	* Return render system state */
	virtual bool CanRender() const = 0;
	/**
	* Return render system type */
	virtual LcRenderSystemType GetType() const = 0;


protected:
	/**
	* Render sprite */
	virtual void RenderSprite(const LcSpriteData& sprite) = 0;


protected:
	class IApplication& app;

};
