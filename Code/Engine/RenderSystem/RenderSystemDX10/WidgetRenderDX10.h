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
	LcWidgetRenderDX10(float inDpi) : dpi(inDpi), features{EVCType::Texture} {}
	//
	~LcWidgetRenderDX10();
	//
	void Init(const LcAppContext& context);
	//
	void Shutdown();
	//
	void RemoveFonts() { fonts.clear(); }
	//
	void RenderText(const std::wstring& text, LcRectf rect, LcColor4 color, const ITextFont* font,
		ID2D1RenderTarget* target, const LcAppContext& context);
	//
	void CreateTextureAndRenderTarget(class LcWidgetDX10& widget, LcVector2 scale, const LcAppContext& context);
	//
	inline int GetNumFonts() const { return (int)fonts.size(); }
	//
	inline const TVFeaturesList& GetFeaturesList() const { return features; }


public: // IWidgetRender interface implementation
	//
	virtual const ITextFont* AddFont(const std::wstring& fontName, float fontSize, LcFontWeight fontWeight = LcFontWeight::Normal) override;
	//
	virtual bool RemoveFont(const ITextFont* font) override;


public: // IVisual2DRender interface implementation
	//
	virtual void Setup(const IVisual* visual, const LcAppContext& context) override;
	//
	virtual void RenderWidget(const IWidget* widget, const LcAppContext& context) override;


protected:
	TVFeaturesList features;
	//
	ComPtr<ID2D1Factory> d2dFactory;
	//
	ComPtr<IDWriteFactory> dwriteFactory;
	//
	std::map<std::wstring, std::shared_ptr<ITextFont>> fonts;
	//
	float dpi;

};
