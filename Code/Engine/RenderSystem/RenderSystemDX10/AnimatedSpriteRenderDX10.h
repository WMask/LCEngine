/**
* AnimatedSpriteRenderDX10.h
* 27.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/SpriteRender.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"


/**
* Animated sprite render */
class LcAnimatedSpriteRenderDX10 : public ISpriteRender
{
public:
	/**
	* Constructor */
	LcAnimatedSpriteRenderDX10(IRenderDeviceDX10& renderDevice);
	/**
	* Destructor */
	~LcAnimatedSpriteRenderDX10();


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
