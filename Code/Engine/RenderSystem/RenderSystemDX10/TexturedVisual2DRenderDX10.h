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
	LcTexturedVisual2DRenderDX10(IRenderDeviceDX10& renderDevice);
	/**
	* Destructor */
	~LcTexturedVisual2DRenderDX10();


public:// ISpriteRender interface implementation
	/**
	* Setup render state */
	virtual void Setup() override;
	/**
	* Render sprite */
	virtual void RenderSprite(const class ISprite* sprite) override;
	/**
	* Render widget */
	virtual void RenderWidget(const class IWidget* widget) override;
	/**
	* Checks support for the feature */
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
