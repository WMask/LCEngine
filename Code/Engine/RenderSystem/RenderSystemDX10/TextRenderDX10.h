/**
* TextRenderDX10.h
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

#include "Core/LCTypesEx.h"
#include "Core/Visual.h"

using Microsoft::WRL::ComPtr;


/** Text font */
struct ITextFontDX10 : public ITextFont
{
public:
	virtual void SetAlignment(LcTextAlignment align) = 0;
	//
	virtual IDWriteTextFormat* GetFont() const = 0;
};


/**
* Text renderer */
class LcTextRenderDX10
{
public:
	LcTextRenderDX10(float inDpi) : dpi(inDpi), features{EVCType::Texture} {}
	//
	~LcTextRenderDX10();
	//
	void Init(const LcAppContext& context);
	//
	void Shutdown();
	//
	const ITextFont* AddFont(const std::wstring& fontName, float fontSize, LcFontWeight fontWeight = LcFontWeight::Normal);
	//
	bool RemoveFont(const ITextFont* font);
	//
	void RemoveFonts() { fonts.clear(); }
	//
	void RenderText(const std::wstring& text, LcRectf rect, LcColor4 color, LcTextAlignment align,
		const ITextFont* font, ID2D1RenderTarget* target, const LcAppContext& context);
	//
	void CreateTextureAndRenderTarget(class LcWidgetDX10& widget, LcVector2 scale, const LcAppContext& context);
	//
	inline int GetNumFonts() const { return (int)fonts.size(); }


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
