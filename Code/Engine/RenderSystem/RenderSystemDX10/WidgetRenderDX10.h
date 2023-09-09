/**
* WidgetRenderDX10.h
* 30.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include <d3d10_1.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wrl.h>
#include <memory>
#include <string>
#include <map>

#include "RenderSystem/WidgetRender.h"
#include "Core/LCTypesEx.h"

using Microsoft::WRL::ComPtr;


/** Text font */
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
	LcWidgetRenderDX10(class IRenderDeviceDX10& devicePtr, HWND hWndPtr) : textureRender(nullptr),
		device(devicePtr), hWnd(hWndPtr), renderMode(EWRMode::Textures), features{EVCType::Texture} {}
	//
	~LcWidgetRenderDX10();
	//
	void Shutdown();
	//
	bool RemoveFont(const ITextFont* font);
	//
	void RenderText(const std::wstring& text, const LcRectf& rect, const LcColor4& color, const ITextFont* font);
	//
	void BeginRender();
	//
	long EndRender();
	//
	inline IVisual2DRender* GetTextureRender() const { return textureRender; }
	//
	inline const TVFeaturesList& GetFeaturesList() const { return features; }


public:// IWidgetRender interface implementation
	//
	const ITextFont* AddFont(const std::wstring& fontName, unsigned short fontSize, LcFontWeight fontWeight = LcFontWeight::Normal);
	//
	virtual void Setup(const IVisual* visual) override;
	//
	virtual void RenderWidget(const IWidget* widget) override;
	//
	virtual EWRMode GetRenderMode() const override { return renderMode; }
	//
	virtual void SetRenderMode(EWRMode inRenderMode) override { renderMode = inRenderMode; }


protected:
	HWND hWnd;
	//
	EWRMode renderMode;
	//
	TVFeaturesList features;
	//
	class IRenderDeviceDX10& device;
	//
	IVisual2DRender* textureRender;
	//
	ComPtr<ID2D1Factory> d2dFactory;
	//
	ComPtr<ID2D1RenderTarget> renderTarget;
	//
	ComPtr<IDWriteFactory> dwriteFactory;
	//
	std::map<std::wstring, std::shared_ptr<ITextFont>> fonts;

};
