/**
* UtilsDX10.h
* 26.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include <d3d10_1.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl.h>
#include <string>
#include <map>

#include "RenderSystem/WidgetRender.h"
#include "World/WorldInterface.h"
#include "Core/LCTypesEx.h"

using Microsoft::WRL::ComPtr;


/**
* Texture loader */
class LcTextureLoaderDX10
{
public:
	LcTextureLoaderDX10(TWeakWorld worldPtr) : world(worldPtr) {}
	//
	~LcTextureLoaderDX10();
	//
	bool LoadTexture(const char* texPath, ID3D10Device1* device, ID3D10Texture2D** texture, ID3D10ShaderResourceView1** view, LcSize* outTexSize);
	/** If world is not null - only unused textures removed. If null - all textures removed. */
	void ClearCache(IWorld* world);


protected:
	struct LcTextureDataDX10
	{
		ComPtr<ID3D10Texture2D> texture;
		//
		ComPtr<ID3D10ShaderResourceView1> view;
		//
		LcSize texSize = LcSize();
	};
	//
	std::map<std::string, LcTextureDataDX10> texturesCache;
	//
	ComPtr<IWICImagingFactory2> factory;
	//
	TWeakWorld world;

};


/**
* Text font */
struct ITextFontDX10 : public ITextFont
{
public:
	virtual IDWriteTextFormat* GetFont() const = 0;
};

/**
* Widget renderer */
class LcWidgetRenderDX10 : public IWidgetRender
{
public:
	LcWidgetRenderDX10(IDXGISwapChain* swapChainPtr, HWND hWnd);
	//
	~LcWidgetRenderDX10();
	//
	void BeginRender();
	//
	HRESULT EndRender();


public: // IWidgetRender interface implementation
	//
	virtual const ITextFont* AddFont(const std::wstring& fontName, unsigned short fontSize, LcFontWeight fontWeight = LcFontWeight::Normal);
	//
	virtual bool RemoveFont(const ITextFont* font) override;
	//
	virtual void RenderText(const std::wstring& text, const LcRectf& rect, const LcColor4& color, const ITextFont* font);


protected:
	IDXGISwapChain* swapChain;
	//
	ComPtr<ID2D1Factory> d2dFactory;
	//
	ComPtr<ID2D1RenderTarget> renderTarget;
	//
	ComPtr<IDWriteFactory> dwriteFactory;
	//
	std::map<std::wstring, std::shared_ptr<ITextFont>> fonts;
	//
	unsigned int screenHeight;

};
