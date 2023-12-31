/**
* AnimatedSpriteRenderDX10.h
* 27.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"


/**
* Animated sprite render */
class LcAnimatedSpriteRenderDX10 : public IVisual2DRender
{
public:
	//
	LcAnimatedSpriteRenderDX10(const LcAppContext& context);
	//
	~LcAnimatedSpriteRenderDX10();


public:// IVisual2DRender interface implementation
	//
	virtual void Setup(const IVisual* visual, const LcAppContext& context) override;
	//
	virtual void Render(const IVisual* visual, const LcAppContext& context) override;
	//
	virtual bool Supports(const TVFeaturesList& features) const override;


protected:
	//
	ID3D10Buffer* vertexBuffer;
	//
	ID3D10InputLayout* vertexLayout;
	//
	ID3D10VertexShader* vs;
	//
	ID3D10PixelShader* ps;

};
