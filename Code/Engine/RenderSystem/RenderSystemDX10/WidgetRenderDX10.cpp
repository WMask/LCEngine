/**
* WidgetRenderDX10.cpp
* 30.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/WidgetRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "RenderSystem/RenderSystemDX10/VisualsDX10.h"
#include "Core/LCException.h"

#include <sstream>


LcWidgetRenderDX10::~LcWidgetRenderDX10()
{
    Shutdown();
}

void LcWidgetRenderDX10::Shutdown()
{
    dwriteFactory.Reset();
    d2dFactory.Reset();
}

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
    unsigned short fontSize = 0;
    //
    ComPtr<IDWriteTextFormat> font;
};

inline bool operator==(const LC_FONT_DATA& a, const LC_FONT_DATA& b)
{
    return a.fontName == b.fontName && a.fontSize == b.fontSize && a.fontWeight == b.fontWeight;
}

struct LcTextFontDX10 : public ITextFontDX10
{
public:
    LcTextFontDX10(const std::wstring& fontName, unsigned short fontSize, LcFontWeight fontWeight, IDWriteFactory* dwriteFactory)
    {
        LC_TRY

        if (!dwriteFactory) throw std::exception("LcTextFont(): Invalid DWrite factory");

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
            throw std::exception("LcTextFont(): Cannot create font");
        }

        data.font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        data.font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        LC_CATCH{ LC_THROW("LcTextFontDX10()") }
    }
    //
    virtual ~LcTextFontDX10() override {}
    //
    virtual IDWriteTextFormat* GetFont() const override { return data.font.Get(); }
    //
    virtual std::wstring GetFontName() const override
    {
        std::wstringstream ss;
        ss << data.fontName << L"_" << data.fontSize << L"_" << (int)data.fontWeight;
        return ss.str();
    }
    //
    virtual const LC_FONT_DATA& GetData() const { return data; }

protected:
    LC_FONT_DATA data;
};

const ITextFont* LcWidgetRenderDX10::AddFont(const std::wstring& fontName, unsigned short fontSize, LcFontWeight fontWeight)
{
    std::shared_ptr<LcTextFontDX10> newFont;

    LC_TRY

    if (!dwriteFactory) throw std::exception("LcWidgetRenderDX10::Setup(): Invalid DirectWrite factory");

    for (auto entry : fonts)
    {
        auto font = (const LcTextFontDX10*)entry.second.get();
        if (font->GetData() == LC_FONT_DATA{fontName, fontWeight, fontSize}) return entry.second.get();
    }

    newFont = std::make_shared<LcTextFontDX10>(fontName, fontSize, fontWeight, dwriteFactory.Get());
    fonts[newFont->GetFontName()] = newFont;

    LC_CATCH{ LC_THROW("LcWidgetRenderDX10::AddFont()") }

    return newFont.get();
}

bool LcWidgetRenderDX10::RemoveFont(const ITextFont* font)
{
    if (auto fontPtr = (const LcTextFontDX10*)font)
    {
        return fonts.erase(fontPtr->GetFontName()) == 1;
    }

    return false;
}

void LcWidgetRenderDX10::Init(const LcAppContext& context)
{
    LC_TRY

    auto render = static_cast<LcRenderSystemDX10*>(context.render);
    auto swapChain = render ? render->GetD3D10SwapChain() : nullptr;
    if (!hWnd || !swapChain) throw std::exception("LcWidgetRenderDX10::Init(): Invalid arguments");

    if (d2dFactory) Shutdown();

    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf())))
    {
        throw std::exception("LcWidgetRenderDX10::Init(): Cannot create Direct2D factory");
    }

    ComPtr<IDXGISurface1> backBuffer;
    if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
    {
        throw std::exception("LcWidgetRenderDX10::Init(): Cannot get back buffer");
    }

    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(dwriteFactory.Get()),
        reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf()))))
    {
        throw std::exception("LcWidgetRenderDX10::Init(): Cannot create DirectWrite factory");
    }

    LC_CATCH{ LC_THROW("LcWidgetRenderDX10::Init()") }
}

void LcWidgetRenderDX10::Setup(const IVisual* visual, const LcAppContext& context)
{
    auto render = static_cast<LcRenderSystemDX10*>(context.render);
    auto textureRender = render ? render->GetTextureRender() : nullptr;
    if (!textureRender) throw std::exception("LcWidgetRenderDX10::Setup(): Invalid renderer");

    textureRender->Setup(visual, context);
}

void LcWidgetRenderDX10::RenderWidget(const IWidget* widget, const LcAppContext& context)
{
    auto render = static_cast<LcRenderSystemDX10*>(context.render);
    auto textureRender = render ? render->GetTextureRender() : nullptr;
    if (!textureRender) throw std::exception("LcWidgetRenderDX10::RenderWidget(): Invalid renderer");

    textureRender->RenderWidget(widget, context);
}

void LcWidgetRenderDX10::RenderText(const std::wstring& text, const LcRectf& rect, const LcColor4& color, const ITextFont* font,
    ID2D1RenderTarget* target, const LcAppContext& context)
{
    LC_TRY

    if (!target) throw std::exception("LcWidgetRenderDX10::RenderText(): Invalid renderer");
    if (!font) throw std::exception("LcWidgetRenderDX10::RenderText(): Invalid font");

    D2D1_RECT_F frect{ rect.left, rect.top, rect.right, rect.bottom };
    D2D1_COLOR_F fcolor{ color.x, color.y, color.z, color.w };
    auto fontDX10 = (ITextFontDX10*)font;

    ComPtr<ID2D1SolidColorBrush> brush;
    target->CreateSolidColorBrush(fcolor, brush.GetAddressOf());

    target->BeginDraw();
    target->DrawTextW(text.c_str(), (UINT32)text.length(), fontDX10->GetFont(), frect, brush.Get());
    target->EndDraw();

    LC_CATCH{ LC_THROW("LcWidgetRenderDX10::RenderText()") }
}

void LcWidgetRenderDX10::CreateTextureAndRenderTarget(LcWidgetDX10& widget, const LcAppContext& context)
{
    auto renderDX10 = static_cast<LcRenderSystemDX10*>(context.render);
    auto d3dDevice = renderDX10 ? renderDX10->GetD3D10Device() : nullptr;
    if (!d3dDevice) throw std::exception("LcWidgetRenderDX10::CreateTextureAndRenderTarget(): Invalid render system");

    D3D10_TEXTURE2D_DESC texDesc{};
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.Width = static_cast<UINT>(widget.GetSize().x + 0.01f);
    texDesc.Height = static_cast<UINT>(widget.GetSize().y + 0.01f);

    if (FAILED(d3dDevice->CreateTexture2D(&texDesc, NULL, widget.textTexture.GetAddressOf())))
    {
        throw std::exception("LcWidgetRenderDX10::CreateTextureAndRenderTarget(): Cannot create text texture");
    }

    D3D10_SHADER_RESOURCE_VIEW_DESC1 SRVDesc{};
    SRVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;

    if (FAILED(d3dDevice->CreateShaderResourceView1(widget.textTexture.Get(), &SRVDesc, widget.textTextureSV.GetAddressOf())))
    {
        throw std::exception("LcWidgetRenderDX10::CreateTextureAndRenderTarget(): Cannot create shader resource view");
    }

    ComPtr<IDXGISurface1> textSurface;
    if (FAILED(widget.textTexture->QueryInterface(textSurface.GetAddressOf())))
    {
        throw std::exception("LcWidgetRenderDX10::CreateTextureAndRenderTarget(): Cannot create text surface");
    }

    float dpi = (float)GetDpiForWindow(hWnd);
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi, dpi);

    if (FAILED(d2dFactory->CreateDxgiSurfaceRenderTarget(textSurface.Get(), &props, widget.textRenderTarget.GetAddressOf())))
    {
        throw std::exception("LcWidgetRenderDX10::CreateTextureAndRenderTarget(): Cannot create render target");
    }
}
