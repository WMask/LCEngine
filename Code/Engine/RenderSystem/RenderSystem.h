/**
* RenderSystem.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LcTypes.h"
#include "Module.h"

#include <map>

#pragma warning(disable : 4251)


/**
* Render system interface */
class RENDERSYSTEM_API IRenderSystem
{
public:
	typedef std::map<std::string, std::string> SHADERS_MAP;


public:
	/**
	* Virtual destructor */
	virtual ~IRenderSystem();
	/**
	* Load shaders */
	virtual void LoadShaders(const std::string& folderPath);
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
	virtual void RenderSprite(const class ISprite* sprite) = 0;


protected:
	SHADERS_MAP shaders;

};
