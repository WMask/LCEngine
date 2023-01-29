/**
* RenderSystem.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "RenderSystemModule.h"
#include "Core/LCTypes.h"


/**
* Render system type */
enum class ERenderSystemType
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
	/**
	* Virtual destructor */
	virtual ~IRenderSystem();
	/**
	* Create render system */
	virtual void Create(void* Handle, LCSize viewportSize, bool windowed) = 0;
	/**
	* Shutdown render system */
	virtual void Shutdown() = 0;
	/**
	* Update world */
	virtual void Update(float deltaSeconds) = 0;
	/**
	* Render world */
	virtual void Render(float deltaSeconds) = 0;
	/**
	* Return render system state */
	virtual bool CanRender() const = 0;
	/**
	* Return render system type */
	virtual ERenderSystemType GetType() const = 0;

};
