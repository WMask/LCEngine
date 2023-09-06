/**
* UtilsDX10.h
* 26.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include <d3d10_1.h>
#include <d2d1.h>
#include <wincodec.h>
#include <wrl.h>
#include <string>
#include <map>

#include "World/WorldInterface.h"
#include "World/Sprites.h"
#include "GUI/Widgets.h"
#include "Core/LCTypesEx.h"

using Microsoft::WRL::ComPtr;


/** Enumerate adapters */
std::vector<ComPtr<IDXGIAdapter>> LcEnumerateAdapters();

/** Find display mode */
bool LcFindDisplayMode(int width, int height, DXGI_MODE_DESC* outMode);

/** Make window association to enable fullscreen mode on Alt + Enter */
void LcMakeWindowAssociation(HWND hWnd);


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
* DirectX10 Sprite implementation */
class LcSpriteDX10 : public LcSprite
{
public:
	LcSpriteDX10(LcSpriteData inSprite, class LcRenderSystemDX10& inRender) : LcSprite(inSprite), render(inRender) {}
	//
	class LcRenderSystemDX10& render;
	//
	ComPtr<ID3D10Texture2D> texture;
	//
	ComPtr<ID3D10ShaderResourceView1> shaderView;


public:// IVisual interface implementation
	virtual void AddComponent(TVComponentPtr comp) override;

};


/**
* DirectX10 Widget implementation */
class LcWidgetDX10 : public LcWidget
{
public:
	LcWidgetDX10(LcWidgetData inSprite, class LcRenderSystemDX10& inRender) : LcWidget(inSprite), render(inRender), font(nullptr) {}
	//
	class LcRenderSystemDX10& render;
	//
	ComPtr<ID3D10Texture2D> texture;
	//
	ComPtr<ID3D10ShaderResourceView1> shaderView;
	//
	const ITextFont* font;


public:// IVisual interface implementation
	virtual void AddComponent(TVComponentPtr comp) override;

};
