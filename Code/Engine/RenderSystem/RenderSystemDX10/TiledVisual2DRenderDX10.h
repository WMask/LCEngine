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
class LcTiledVisual2DRenderDX10 : public IVisual2DRender
{
public:
	//
	LcTiledVisual2DRenderDX10(const LcAppContext& context);
	//
	~LcTiledVisual2DRenderDX10();
	//
	void RemoveTiles(const IVisual* visual) { vertexBuffers.erase(visual); }
	//
	void RemoveTiles() { vertexBuffers.clear(); }
	//
	void ClearCache(IWorld* world);
	//
	inline int GetNumTiles() const { return (int)vertexBuffers.size(); }


public:// IVisual2DRender interface implementation
	//
	virtual void Setup(const IVisual* visual, const LcAppContext& context) override;
	//
	virtual void Render(const IVisual* visual, const LcAppContext& context) override;
	//
	virtual bool Supports(const TVFeaturesList& features) const override;


protected:
	//
	LcTileBuffersList vertexBuffers;
	//
	ID3D10InputLayout* vertexLayout;
	//
	ID3D10VertexShader* vs;
	//
	ID3D10PixelShader* ps;

};
