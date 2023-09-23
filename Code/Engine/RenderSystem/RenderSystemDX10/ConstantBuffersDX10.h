/**
* ConstantBuffersDX10.h
* 23.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypesEx.h"

#include <d3d10_1.h>
#include <wrl.h>
#include <string>
#include <map>

using Microsoft::WRL::ComPtr;


struct VS_SETTINGS_BUFFER
{
	LcVector4 worldTint;
};

/**
* Constant buffers */
struct LcConstantBuffersDX10
{
public:
	LcConstantBuffersDX10() {}
	//
	void Init(ID3D10Device1* d3dDevice, LcVector3 cameraPos, LcVector3 cameraTarget, int width, int height);
	//
	void Destroy();


public:
	//
	ComPtr<ID3D10Buffer> projMatrixBuffer;
	//
	ComPtr<ID3D10Buffer> transMatrixBuffer;
	//
	ComPtr<ID3D10Buffer> viewMatrixBuffer;
	//
	ComPtr<ID3D10Buffer> colorsBuffer;
	//
	ComPtr<ID3D10Buffer> customUvBuffer;
	//
	ComPtr<ID3D10Buffer> frameAnimBuffer;
	//
	ComPtr<ID3D10Buffer> settingsBuffer;

};
