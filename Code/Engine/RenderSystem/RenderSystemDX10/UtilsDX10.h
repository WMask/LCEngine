/**
* UtilsDX10.h
* 26.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include <d3d10.h>
#include <wincodec.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;


/**
* Texture loader */
class LcTextureLoaderDX10
{
public:
	LcTextureLoaderDX10();
	//
	~LcTextureLoaderDX10();
	//
	bool LoadTexture(const char* texPath, ID3D10Device* device, ID3D10Texture2D** texture, ID3D10ShaderResourceView** view);


protected:
	ComPtr<IWICImagingFactory2> factory;

};
