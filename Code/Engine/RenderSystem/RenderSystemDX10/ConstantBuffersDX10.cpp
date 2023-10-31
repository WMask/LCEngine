/**
* ConstantBuffersDX10.cpp
* 23.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/ConstantBuffersDX10.h"
#include "Core/LCException.h"
#include "Core/LCTypesEx.h"


void LcConstantBuffersDX10::Init(ID3D10Device1* d3dDevice, LcVector3 cameraPos, LcVector3 cameraTarget, int width, int height)
{
	LC_TRY

	struct VS_MATRIX_BUFFER
	{
		LcMatrix4 mat;
	};
	VS_MATRIX_BUFFER matData;

	struct VS_COLORS_BUFFER
	{
		LcColor4 colors[4];
	};
	VS_COLORS_BUFFER colorsData;

	struct VS_CUSTOM_UV_BUFFER
	{
		LcVector4 uv[4];
	};
	VS_CUSTOM_UV_BUFFER uvData;

	struct VS_FRAME_ANIM2D_BUFFER
	{
		LcVector4 animData;
	};
	VS_FRAME_ANIM2D_BUFFER anim2dData;

	VS_SETTINGS_BUFFER settingsData{};
	VS_FLAGS_BUFFER flagsData{};

	D3D10_BUFFER_DESC cbDesc{};
	cbDesc.ByteWidth = sizeof(VS_MATRIX_BUFFER);
	cbDesc.Usage = D3D10_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;

	D3D10_SUBRESOURCE_DATA subResData{};
	subResData.pSysMem = &matData;

	// create constant buffers
	matData.mat = OrthoMatrix(LcSize{ width, height }, 1.0f, -1.0f);
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, projMatrixBuffer.GetAddressOf())))
	{
		throw std::exception("LcConstantBuffersDX10::Init(): Cannot create constant buffer");
	}

	matData.mat = IdentityMatrix();
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, transMatrixBuffer.GetAddressOf())))
	{
		throw std::exception("LcConstantBuffersDX10::Init(): Cannot create constant buffer");
	}

	matData.mat = LookAtMatrix(cameraPos, cameraTarget);
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, viewMatrixBuffer.GetAddressOf())))
	{
		throw std::exception("LcConstantBuffersDX10::Init(): Cannot create constant buffer");
	}

	subResData.pSysMem = &colorsData;
	colorsData.colors[0] = LcDefaults::White4;
	colorsData.colors[1] = LcDefaults::White4;
	colorsData.colors[2] = LcDefaults::White4;
	colorsData.colors[3] = LcDefaults::White4;
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, colorsBuffer.GetAddressOf())))
	{
		throw std::exception("LcConstantBuffersDX10::Init(): Cannot create constant buffer");
	}

	subResData.pSysMem = &uvData;
	uvData.uv[1] = To4(LcVector2{ 1.0, 0.0 });
	uvData.uv[2] = To4(LcVector2{ 1.0, 1.0 });
	uvData.uv[0] = To4(LcVector2{ 0.0, 0.0 });
	uvData.uv[3] = To4(LcVector2{ 0.0, 1.0 });
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, customUvBuffer.GetAddressOf())))
	{
		throw std::exception("LcConstantBuffersDX10::Init(): Cannot create constant buffer");
	}

	cbDesc.ByteWidth = sizeof(VS_FRAME_ANIM2D_BUFFER);
	subResData.pSysMem = &anim2dData;
	anim2dData.animData = LcVector4{ 1.0, 1.0, 0.0, 0.0 };
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, frameAnimBuffer.GetAddressOf())))
	{
		throw std::exception("LcConstantBuffersDX10::Init(): Cannot create constant buffer");
	}

	cbDesc.ByteWidth = sizeof(VS_SETTINGS_BUFFER);
	subResData.pSysMem = &settingsData;
	settingsData.worldTint = LcVector4{ 1.0, 1.0, 1.0, 1.0 };
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, settingsBuffer.GetAddressOf())))
	{
		throw std::exception("LcConstantBuffersDX10::Init(): Cannot create constant buffer");
	}

	cbDesc.ByteWidth = sizeof(VS_FLAGS_BUFFER);
	subResData.pSysMem = &flagsData;
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, flagsBuffer.GetAddressOf())))
	{
		throw std::exception("LcConstantBuffersDX10::Init(): Cannot create constant buffer");
	}

	// set buffers
	d3dDevice->VSSetConstantBuffers(0, 1, projMatrixBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(1, 1, viewMatrixBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(2, 1, transMatrixBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(3, 1, colorsBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(4, 1, customUvBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(5, 1, frameAnimBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(6, 1, settingsBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(7, 1, flagsBuffer.GetAddressOf());

	LC_CATCH{ LC_THROW("LcConstantBuffersDX10::Init()") }
}

void LcConstantBuffersDX10::Destroy()
{
	flagsBuffer.Reset();
	settingsBuffer.Reset();
	frameAnimBuffer.Reset();
	customUvBuffer.Reset();
	colorsBuffer.Reset();
	transMatrixBuffer.Reset();
	projMatrixBuffer.Reset();
	viewMatrixBuffer.Reset();
}
