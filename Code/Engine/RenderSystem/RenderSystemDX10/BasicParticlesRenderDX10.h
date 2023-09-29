/**
* BasicParticlesRenderDX10.h
* 29.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"


struct LC_PARTICLES_BUFFER
{
	ComPtr<ID3D10Buffer> buffer;
	int vertexCount;
	float creationTime;
};

typedef std::map<const IVisual*, LC_PARTICLES_BUFFER> LcParticleBuffersList;


/**
* Basic particles render */
class LcBasicParticlesRenderDX10 : public IVisual2DRender
{
public:
	/**
	* Constructor */
	LcBasicParticlesRenderDX10(const LcAppContext& context);
	/**
	* Destructor */
	~LcBasicParticlesRenderDX10();
	/**
	* Removes tile's vertex buffer */
	void RemoveTiles(const IVisual* visual);
	/**
	* Removes all tiles */
	void RemoveTiles() { vertexBuffers.clear(); }
	/**
	* Get tiles count */
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
	LcParticleBuffersList vertexBuffers;
	//
	ID3D10InputLayout* vertexLayout;
	//
	ID3D10VertexShader* vs;
	//
	ID3D10PixelShader* ps;

};
