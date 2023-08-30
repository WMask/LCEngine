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
#include <string>
#include <map>

#include "RenderSystem/WidgetRender.h"
#include "Core/LCTypesEx.h"

using Microsoft::WRL::ComPtr;


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


public:// IWidgetRender interface implementation
	//
	virtual const ITextFont* AddFont(const std::wstring& fontName, unsigned short fontSize, LcFontWeight fontWeight = LcFontWeight::Normal) override;
	//
	virtual bool RemoveFont(const ITextFont* font) override;
	//
	virtual void RenderText(const std::wstring& text, const LcRectf& rect, const LcColor4& color, const ITextFont* font) override;
	//
	virtual void BeginRender() override;
	//
	virtual long EndRender() override;


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
