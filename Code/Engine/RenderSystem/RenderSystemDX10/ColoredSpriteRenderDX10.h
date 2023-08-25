/**
* ColoredSpriteRenderDX10.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/SpriteRender.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"


/**
* Render system interface */
class LcColoredSpriteRenderDX10 : public ISpriteRender
{
public:
	/**
	* Constructor */
	LcColoredSpriteRenderDX10(IRenderDeviceDX10& renderDevice);
	/**
	* Destructor */
	~LcColoredSpriteRenderDX10();


public: // ISpriteRender interface implementation
	/**
	* Setup render state */
	virtual void Setup() override;
	/**
	* Render sprite */
	virtual void Render(const ISprite* sprite) override;
	/**
	* Return sprite type */
	virtual LcSpriteType GetType() const override { return LcSpriteType::Colored; }


protected:
	IRenderDeviceDX10& renderDevice;
	//
	ID3D10Buffer* vertexBuffer;
	//
	ID3D10InputLayout* vertexLayout;
	//
	ID3D10VertexShader* vs;
	//
	ID3D10PixelShader* ps;

};
