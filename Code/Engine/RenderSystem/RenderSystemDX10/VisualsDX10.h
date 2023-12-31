/**
* VisualsDX10.h
* 16.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include <d3d10_1.h>
#include <d2d1.h>
#include <wrl.h>
#include <string>
#include <map>

#include "GUI/Widgets.h"
#include "World/Sprites.h"
#include "Core/LCTypesEx.h"

using Microsoft::WRL::ComPtr;


/**
* DirectX10 Sprite implementation */
class LcSpriteDX10 : public LcSprite
{
public:
	LcSpriteDX10() : texture(nullptr), textureSV(nullptr) {}
	//
	ID3D10Texture2D* texture;
	//
	ID3D10ShaderResourceView1* textureSV;


public:// IVisualBase interface implementation
	virtual void AddComponent(TVComponentPtr comp, const LcAppContext& context) override;
	//
	virtual void Destroy(const LcAppContext& context) override;

};


/**
* DirectX10 Widget implementation */
class LcWidgetDX10 : public LcWidget
{
public:
	LcWidgetDX10() : spriteTexture(nullptr), spriteTextureSV(nullptr), font(nullptr) {}
	//
	void RedrawText(class LcTextRenderDX10* textRender, const LcAppContext& context);


public: // IVisualBase interface implementation
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual void AddComponent(TVComponentPtr comp, const LcAppContext& context) override;
	//
	virtual void RecreateFont(const LcAppContext& context) override;


protected:
	//
	float GetFontSize(const IWidgetTextComponent& textComp, const LcAppContext& context) const;


public:
	//
	ID3D10Texture2D* spriteTexture;
	//
	ID3D10ShaderResourceView1* spriteTextureSV;
	//
	ComPtr<ID3D10Texture2D> textTexture;
	//
	ComPtr<ID3D10ShaderResourceView1> textTextureSV;
	//
	ComPtr<ID2D1RenderTarget> textRenderTarget;
	//
	std::wstring prevRenderedText;
	//
	const ITextFont* font;

};
