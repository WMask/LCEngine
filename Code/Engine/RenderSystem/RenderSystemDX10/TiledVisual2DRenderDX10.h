/**
* TiledVisual2DRenderDX10.h
* 06.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"


struct LC_TILES_BUFFER
{
	ComPtr<ID3D10Buffer> buffer;
	int vertexCount;
};

typedef std::map<const IVisual*, LC_TILES_BUFFER> LcTileBuffersList;


/**
* Textured visual render */
class LcTiledVisual2DRenderDX10 : public ISpriteRender
{
public:
	/**
	* Constructor */
	LcTiledVisual2DRenderDX10(IRenderDeviceDX10& renderDevice);
	/**
	* Destructor */
	~LcTiledVisual2DRenderDX10();


public:// IVisual2DRender interface implementation
	//
	virtual void Setup(const IVisual* visual) override;
	//
	virtual void RenderSprite(const class ISprite* sprite) override;
	//
	virtual bool Supports(const TVFeaturesList& features) const override;


protected:
	IRenderDeviceDX10& renderDevice;
	//
	LcTileBuffersList vertexBuffers;
	//
	ID3D10InputLayout* vertexLayout;
	//
	ID3D10VertexShader* vs;
	//
	ID3D10PixelShader* ps;

};
