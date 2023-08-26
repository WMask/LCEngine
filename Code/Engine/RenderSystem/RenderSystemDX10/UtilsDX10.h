/**
* UtilsDX10.h
* 26.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include <d3d10.h>
#include <wincodec.h>
#include <wrl.h>
#include <string>
#include <map>

#include "World/WorldInterface.h"

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
	bool LoadTexture(const char* texPath, ID3D10Device* device, ID3D10Texture2D** texture, ID3D10ShaderResourceView** view);
	/** If world is not null - only unused textures removed. If null - all textures removed. */
	void ClearCache(IWorld* world);


protected:
	struct LcTextureDataDX10
	{
		ComPtr<ID3D10Texture2D> texture;
		//
		ComPtr<ID3D10ShaderResourceView> view;
	};
	//
	std::map<std::string, LcTextureDataDX10> texturesCache;
	//
	TWeakWorld world;

};
