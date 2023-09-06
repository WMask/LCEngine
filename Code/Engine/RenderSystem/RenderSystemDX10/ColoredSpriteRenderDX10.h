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
* Colored sprite render */
class LcColoredSpriteRenderDX10 : public ISpriteRender
{
public:
	/**
	* Constructor */
	LcColoredSpriteRenderDX10(IRenderDeviceDX10& renderDevice);
	/**
	* Destructor */
	~LcColoredSpriteRenderDX10();


public:// IVisual2DRender interface implementation
	//
	virtual void Setup(const IVisual* visual) override;
	//
	virtual void RenderSprite(const ISprite* sprite) override;
	//
	virtual bool Supports(const TVFeaturesList& features) const override;


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
