/**
* DX10RenderSystem.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <deque>
#include <d3d10.h>

#include "DX10RenderSystemModule.h"
#include "RenderSystem/RenderSystem.h"
#include "RenderSystem/SpriteRender.h"
#include "Core/LCTypes.h"


#pragma warning(disable : 4251)

/**
* Render device */
class DX10RENDERSYSTEM_API IDX10RenderDevice
{
public:
	/**
	* Return D3D10 device */
	virtual ID3D10Device* GetD3D10Device() const = 0;
	/**
	* Return matrix buffer */
	virtual ID3D10Buffer* GetTransformBuffer() const = 0;
	/**
	* Get shader code */
	virtual std::string GetShaderCode(const std::string& shaderName) const = 0;
	/**
	* Return initial sprite offset */
	virtual LcVector2 GetOffset() const = 0;

};


/**
* Transform buffer */
struct VS_TRANS_BUFFER
{
	LcMatrix4 trans;      // scale * rotation * translation
	LcVector4 colors[4]; // colors
	//
	VS_TRANS_BUFFER()
	{
		colors[0] = LcVector4::Ones();
		colors[1] = LcVector4::Ones();
		colors[2] = LcVector4::Ones();
		colors[3] = LcVector4::Ones();
	}
};


/**
* DirectX 10 render system */
class DX10RENDERSYSTEM_API DX10RenderSystem
	: public IRenderSystem
	, public IDX10RenderDevice
{
public:
	DX10RenderSystem(class IApplication& app);


public: // IRenderSystem interface implementation
	/**
	* Virtual destructor */
	virtual ~DX10RenderSystem() override;
	/**
	* Create render system */
	virtual void Create(void* Handle, LcSize viewportSize, bool windowed) override;
	/**
	* Shutdown render system */
	virtual void Shutdown() override;
	/**
	* Update world */
	virtual void Update(float deltaSeconds) override;
	/**
	* Render world */
	virtual void Render() override;
	/**
	* Render world */
	virtual void RenderSprite(const SPRITE_DATA& sprite) override;
	/**
	* Return render system state */
	virtual bool CanRender() const override { return d3dDevice; }
	/**
	* Return render system type */
	virtual ERenderSystemType GetType() const override { return ERenderSystemType::DX10; }


public: // IDX10RenderDevice interface implementation
	/**
	* Return D3D10 device */
	virtual ID3D10Device* GetD3D10Device() const override { return d3dDevice; }
	/**
	* Return matrix buffer */
	virtual ID3D10Buffer* GetTransformBuffer() const override { return transMatrixBuffer; }
	/**
	* Get shader code */
	virtual std::string GetShaderCode(const std::string& shaderName) const override;
	/**
	* Return initial sprite offset */
	virtual LcVector2 GetOffset() const override { return initialOffset; }


protected:
	ID3D10Device* d3dDevice;
	//
	IDXGISwapChain* swapChain;
	//
	ID3D10RenderTargetView* renderTargetView;
	//
	ID3D10Buffer* projMatrixBuffer;
	//
	ID3D10Buffer* transMatrixBuffer;
	//
	ID3D10BlendState* blendState;
	//
	ID3D10RasterizerState* rasterizerState;
	//
	std::deque<std::shared_ptr<ISpriteRender>> spriteRenders;
	//
	LcVector2 initialOffset;

};
