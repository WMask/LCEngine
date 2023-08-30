/**
* LcTexturedSpriteRenderDX10.h
* 25.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/SpriteRender.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"


/**
* Render system interface */
class LcTexturedSpriteRenderDX10 : public ISpriteRender
{
public:
	/**
	* Constructor */
	LcTexturedSpriteRenderDX10(IRenderDeviceDX10& renderDevice);
	/**
	* Destructor */
	~LcTexturedSpriteRenderDX10();


public: // ISpriteRender interface implementation
	/**
	* Setup render state */
	virtual void Setup() override;
	/**
	* Render sprite */
	virtual void Render(const ISprite* sprite) override;
	/**
	* Checks support for the feature */
	virtual bool Supports(const TSFeaturesList& features) const override;


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
