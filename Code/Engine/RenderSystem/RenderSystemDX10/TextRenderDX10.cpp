/**
* TextRenderDX10.cpp
* 30.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/TextRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "RenderSystem/RenderSystemDX10/VisualsDX10.h"
#include "Core/LCLocalization.h"
#include "Core/LCException.h"

#include <sstream>


DWRITE_FONT_WEIGHT ConvertDWeight(LcFontWeight weight)
{
    switch (weight)
    {
    case LcFontWeight::Light: return DWRITE_FONT_WEIGHT_LIGHT;
    case LcFontWeight::Bold: return DWRITE_FONT_WEIGHT_BOLD;
    }

    return DWRITE_FONT_WEIGHT_NORMAL;
}

struct LC_FONT_DATA
{
    std::wstring fontName;
    //
    LcFontWeight fontWeight = LcFontWeight::Normal;
    //
    float fontSize = 0.0f;
    //
    ComPtr<IDWriteTextFormat> font;
};

inline bool operator==(const LC_FONT_DATA& a, const LC_FONT_DATA& b)
{
    return a.fontName == b.fontName && a.fontSize == b.fontSize && a.fontWeight == b.fontWeight;
}

std::wstring MakeFontName(const std::wstring& fontName, float fontSize, LcFontWeight fontWeight)
{
    std::wstringstream ss;
    ss << fontName << L"_" << fontSize << L"_" << (int)fontWeight;
    return ss.str();
}

struct LcTextFontDX10 : public ITextFontDX10
{
public:
    LcTextFontDX10(const std::wstring& fontName, float fontSize, LcFontWeight fontWeight, IDWriteFactory* dwriteFactory)
    {
        LC_TRY

        if (!dwriteFactory) throw std::exception("LcTextFontDX10(): Invalid DWrite factory");

        data.fontName = fontName;
        data.fontSize = fontSize;
        data.fontWeight = fontWeight;

        if (FAILED(dwriteFactory->CreateTextFormat(
            fontName.c_str(), NULL,
            ConvertDWeight(fontWeight),
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            fontSize, L"", data.font.GetAddressOf())))
        {
            throw std::exception("LcTextFontDX10(): Cannot create font");
        }

        LC_CATCH{ LC_THROW("LcTextFontDX10()") }
    }
    //
    virtual ~LcTextFontDX10() override {}
    //
    virtual void SetAlignment(LcTextAlignment align) override
    {
        switch (align)
        {
        case LcTextAlignment::Left:
            data.font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            data.font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            break;
        case LcTextAlignment::Center:
            data.font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            data.font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            break;
        case LcTextAlignment::Justified:
            data.font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
            data.font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            break;
        case LcTextAlignment::Right:
            data.font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            data.font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
            break;
        }
    }
    //
    virtual IDWriteTextFormat* GetFont() const override { return data.font.Get(); }
    //
    virtual std::wstring GetFontName() const override
    {
        return MakeFontName(data.fontName, data.fontSize, data.fontWeight);
    }
    //
    virtual const LC_FONT_DATA& GetData() const { return data; }

protected:
    LC_FONT_DATA data;
};


LcTextRenderDX10::~LcTextRenderDX10()
{
    Shutdown();
}

void LcTextRenderDX10::Shutdown()
{
    dwriteFactory.Reset();
    d2dFactory.Reset();
}

void LcTextRenderDX10::Init(const LcAppContext& context)
{
    LC_TRY

    auto render = static_cast<LcRenderSystemDX10*>(context.render);
    auto swapChain = render ? render->GetD3D10SwapChain() : nullptr;
    if (!swapChain) throw std::exception("LcTextRenderDX10::Init(): Invalid swap chain");

    if (d2dFactory) Shutdown();

    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf())))
    {
        throw std::exception("LcTextRenderDX10::Init(): Cannot create Direct2D factory");
    }

    ComPtr<IDXGISurface1> backBuffer;
    if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
    {
        throw std::exception("LcTextRenderDX10::Init(): Cannot get back buffer");
    }

    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(dwriteFactory.Get()),
        reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf()))))
    {
        throw std::exception("LcTextRenderDX10::Init(): Cannot create DirectWrite factory");
    }

    context.text->onCultureChanged.AddListener([this](std::string newCulture, const LcAppContext& context) {
        CultureChangedHandler(newCulture, context);
    });

    LC_CATCH{ LC_THROW("LcTextRenderDX10::Init()") }
}

void LcTextRenderDX10::CultureChangedHandler(std::string newCulture, const LcAppContext& context)
{
    auto& visuals = context.world->GetVisuals();
    for (auto& visual : visuals)
    {
        if (visual->GetTypeId() == LcCreatables::Widget)
        {
            auto widget = static_cast<LcWidgetDX10*>(visual.get());
            widget->RedrawText(this, context);
        }
    }
}

const ITextFont* LcTextRenderDX10::AddFont(const std::wstring& fontName, float fontSize, LcFontWeight fontWeight)
{
    std::shared_ptr<LcTextFontDX10> newFont;

    LC_TRY

    if (!dwriteFactory) throw std::exception("LcTextRenderDX10::Setup(): Invalid DirectWrite factory");

    for (auto entry : fonts)
    {
        auto font = (const LcTextFontDX10*)entry.second.get();
        if (font->GetData() == LC_FONT_DATA{fontName, fontWeight, fontSize}) return entry.second.get();
    }

    newFont = std::make_shared<LcTextFontDX10>(fontName, fontSize, fontWeight, dwriteFactory.Get());
    fonts[newFont->GetFontName()] = newFont;

    LC_CATCH{ LC_THROW("LcTextRenderDX10::AddFont()") }

    return newFont.get();
}

bool LcTextRenderDX10::RemoveFont(const ITextFont* font)
{
    if (auto fontPtr = (const LcTextFontDX10*)font)
    {
        return fonts.erase(fontPtr->GetFontName()) == 1;
    }

    return false;
}

void LcTextRenderDX10::ClearCache(IWorld* world)
{
    LC_TRY

    if (world)
    {
        std::set<std::wstring> aliveFontList;
        auto& visuals = world->GetVisuals();
        for (auto visual : visuals)
        {
            auto widget = static_cast<IWidget*>(visual.get());
            auto textComp = widget ? widget->GetTextComponent() : nullptr;
            if (textComp)
            {
                auto& settings = textComp->GetSettings();
                std::wstring name = MakeFontName(settings.fontName, settings.fontSize, settings.fontWeight);
                aliveFontList.insert(name);
            }
        }

        std::set<std::wstring> eraseFontList;
        for (auto font : fonts)
        {
            if (aliveFontList.find(font.first) == aliveFontList.end())
            {
                eraseFontList.insert(font.first);
            }
        }

        for (auto entry : eraseFontList)
        {
            fonts.erase(entry);
        }
    }
    else
    {
        fonts.clear();
    }

    LC_CATCH{ LC_THROW("LcTextRenderDX10::ClearCache()") }
}

void LcTextRenderDX10::RenderText(const std::wstring& text, LcRectf rect, LcColor4 color, LcTextAlignment align,
    const ITextFont* font, ID2D1RenderTarget* target, const LcAppContext& context)
{
    LC_TRY

    auto fontDX10 = (ITextFontDX10*)font;
    auto fontPtr = fontDX10 ? fontDX10->GetFont() : nullptr;

    if (!target) throw std::exception("LcTextRenderDX10::RenderText(): Invalid renderer");
    if (!fontPtr) throw std::exception("LcTextRenderDX10::RenderText(): Invalid font");

    D2D1_RECT_F frect{ rect.left, rect.top, rect.right, rect.bottom };
    D2D1_COLOR_F fcolor{ color.r, color.g, color.b, color.a };

    fontDX10->SetAlignment(align);

    ComPtr<ID2D1SolidColorBrush> brush;
    target->CreateSolidColorBrush(fcolor, brush.GetAddressOf());

    target->BeginDraw();
    target->Clear(D2D1_COLOR_F{1.0f, 1.0f, 1.0f, 0.0f});
    target->DrawTextW(text.c_str(), (UINT32)text.length(), fontPtr, frect, brush.Get());
    target->EndDraw();

    LC_CATCH{ LC_THROW("LcTextRenderDX10::RenderText()") }
}

void LcTextRenderDX10::CreateTextureAndRenderTarget(LcWidgetDX10& widget, LcVector2 scale, const LcAppContext& context)
{
    auto renderDX10 = static_cast<LcRenderSystemDX10*>(context.render);
    auto d3dDevice = renderDX10 ? renderDX10->GetD3D10Device() : nullptr;
    if (!d3dDevice) throw std::exception("LcTextRenderDX10::CreateTextureAndRenderTarget(): Invalid render system");

    D3D10_TEXTURE2D_DESC texDesc{};
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.Width = static_cast<UINT>(widget.GetSize().x * scale.x + 0.01f);
    texDesc.Height = static_cast<UINT>(widget.GetSize().y * scale.y + 0.01f);

    if (FAILED(d3dDevice->CreateTexture2D(&texDesc, NULL, widget.textTexture.GetAddressOf())))
    {
        throw std::exception("LcTextRenderDX10::CreateTextureAndRenderTarget(): Cannot create text texture");
    }

    D3D10_SHADER_RESOURCE_VIEW_DESC1 SRVDesc{};
    SRVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;

    if (FAILED(d3dDevice->CreateShaderResourceView1(widget.textTexture.Get(), &SRVDesc, widget.textTextureSV.GetAddressOf())))
    {
        throw std::exception("LcTextRenderDX10::CreateTextureAndRenderTarget(): Cannot create shader resource view");
    }

    ComPtr<IDXGISurface1> textSurface;
    if (FAILED(widget.textTexture->QueryInterface(textSurface.GetAddressOf())))
    {
        throw std::exception("LcTextRenderDX10::CreateTextureAndRenderTarget(): Cannot create text surface");
    }

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi, dpi);

    if (FAILED(d2dFactory->CreateDxgiSurfaceRenderTarget(textSurface.Get(), &props, widget.textRenderTarget.GetAddressOf())))
    {
        throw std::exception("LcTextRenderDX10::CreateTextureAndRenderTarget(): Cannot create render target");
    }
}
