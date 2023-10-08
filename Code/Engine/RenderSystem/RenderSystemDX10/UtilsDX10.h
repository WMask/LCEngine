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
#include "World/SpriteInterface.h"
#include "GUI/WidgetInterface.h"
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
	LcTextureLoaderDX10() {}
	//
	~LcTextureLoaderDX10();
	//
	bool LoadTexture(const char* texPath, ID3D10Device1* device, ID3D10Texture2D** texture, ID3D10ShaderResourceView1** view, LcSize* outTexSize);
	//
	void RemoveTextures() { texturesCache.clear(); }
	/** If world is not null - only unused textures removed. If null - all textures removed. */
	void ClearCache(IWorld* world);
	//
	inline int GetNumTextures() const { return (int)texturesCache.size(); }


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

};
