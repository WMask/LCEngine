/**
* TexturedVisual2DRenderDX10.h
* 25.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"


/**
* Textured visual render */
class LcTexturedVisual2DRenderDX10 : public ISpriteRender
{
public:
	/**
	* Constructor */
	LcTexturedVisual2DRenderDX10(const LcAppContext& context);
	/**
	* Destructor */
	~LcTexturedVisual2DRenderDX10();


public:// IVisual2DRender interface implementation
	//
	virtual void Setup(const IVisual* visual, const LcAppContext& context) override;
	//
	virtual void RenderSprite(const class ISprite* sprite, const LcAppContext& context) override;
	//
	virtual void RenderWidget(const class IWidget* widget, const LcAppContext& context) override;
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
