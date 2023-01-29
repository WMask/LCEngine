/**
* DX10ColoredSpriteRender.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/SpriteRender.h"
#include "RenderSystem/DX10RenderSystem/DX10RenderSystem.h"


/**
* Render system interface */
class DX10ColoredSpriteRender : public ISpriteRender
{
public:
	/**
	* Constructor */
	DX10ColoredSpriteRender(IDX10RenderDevice& renderDevice);
	/**
	* Destructor */
	~DX10ColoredSpriteRender();


public: // ISpriteRender interface implementation
	/**
	* Setup render state */
	virtual void Setup() override;
	/**
	* Render sprite */
	virtual void Render(const SPRITE_DATA& sprite) override;
	/**
	* Return sprite type */
	virtual ESpriteType GetType() const override { return ESpriteType::Colored; }


protected:
	IDX10RenderDevice& renderDevice;
	//
	ID3D10Buffer* vertexBuffer;
	//
	ID3D10InputLayout* vertexLayout;
	//
	ID3D10VertexShader* vs;
	//
	ID3D10PixelShader* ps;
	//
	VS_TRANS_BUFFER transData;

};
