/**
* WidgetRenderDX10.cpp
* 30.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/WidgetRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"

#include <sstream>


LcWidgetRenderDX10::~LcWidgetRenderDX10()
{
    Shutdown();
}

void LcWidgetRenderDX10::Shutdown()
{
    dwriteFactory.Reset();
    renderTarget.Reset();
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

        data.font.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        data.font.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
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
    if (!dwriteFactory) throw std::exception("LcWidgetRenderDX10::Setup(): Invalid DirectWrite factory");

    for (auto entry : fonts)
    {
        auto font = (const LcTextFontDX10*)entry.second.get();
        if (font->GetData() == LC_FONT_DATA{fontName, fontWeight, fontSize}) return entry.second.get();
    }

    auto newFont = std::make_shared<LcTextFontDX10>(fontName, fontSize, fontWeight, dwriteFactory.Get());
    fonts[newFont->GetFontName()] = newFont;

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

void LcWidgetRenderDX10::Setup()
{
    if (!hWnd || !device.GetD3D10SwapChain()) throw std::exception("LcWidgetRenderDX10::Setup(): Invalid arguments");

    if (d2dFactory) Shutdown();

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    screenHeight = clientRect.bottom - clientRect.top;

    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf())))
    {
        throw std::exception("LcWidgetRenderDX10::Setup(): Cannot create Direct2D factory");
    }

    ComPtr<IDXGISurface1> backBuffer;
    if (FAILED(device.GetD3D10SwapChain()->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
    {
        throw std::exception("LcWidgetRenderDX10::Setup(): Cannot get back buffer");
    }

    float dpi = (float)GetDpiForWindow(hWnd);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi, dpi);

    if (FAILED(d2dFactory->CreateDxgiSurfaceRenderTarget(backBuffer.Get(), &props, renderTarget.GetAddressOf())))
    {
        throw std::exception("LcWidgetRenderDX10::Setup(): Cannot create render target");
    }

    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(dwriteFactory.Get()),
        reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf()))))
    {
        throw std::exception("LcWidgetRenderDX10::Setup(): Cannot create DirectWrite factory");
    }

    auto& renders = device.GetVisual2DRenderList();
    for (auto& render : renders)
    {
        if (render->Supports(features))
        {
            textureRender = render.get();
            break;
        }
    }
}

void LcWidgetRenderDX10::RenderWidget(const IWidget* widgetPtr)
{
    const LcWidgetDX10* widget = (LcWidgetDX10*)widgetPtr;

    switch (renderMode)
    {
    case EWRMode::Textures:
        {
            if (auto button = widget->GetTextureComponent())
            {
                textureRender->RenderWidget(widget);
            }
        }
        break;
    case EWRMode::Text:
        {
            LcRectf rect{
                widget->widget.pos.x - widget->widget.size.x / 2.0f,
                widget->widget.pos.y - widget->widget.size.y / 2.0f,
                widget->widget.pos.x + widget->widget.size.x / 2.0f,
                widget->widget.pos.y + widget->widget.size.y / 2.0f
            };

            if (auto text = widget->GetTextComponent())
            {
                RenderText(text->text, rect, text->textColor, widget->font);
            }
        }
        break;
    }
}

void LcWidgetRenderDX10::RenderText(const std::wstring& text, const LcRectf& rect, const LcColor4& color, const ITextFont* font)
{
    if (!renderTarget) throw std::exception("LcWidgetRenderDX10::RenderText(): Invalid renderer");
    if (!font) throw std::exception("LcWidgetRenderDX10::RenderText(): Invalid font");

    D2D1_RECT_F frect{ rect.left + 0.5f, screenHeight - rect.top + 0.5f, rect.right + 0.5f, screenHeight - rect.bottom + 0.5f };
    D2D1_COLOR_F fcolor{ color.x, color.y, color.z, color.w };
    auto fontDX10 = (ITextFontDX10*)font;

    ComPtr<ID2D1SolidColorBrush> brush;
    renderTarget->CreateSolidColorBrush(fcolor, brush.GetAddressOf());
    renderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), fontDX10->GetFont(), frect, brush.Get());
}

void LcWidgetRenderDX10::BeginRender()
{
    if (renderTarget) renderTarget->BeginDraw();
}

HRESULT LcWidgetRenderDX10::EndRender()
{
    if (renderTarget)
        return renderTarget->EndDraw();
    else
        return D2DERR_INVALID_CALL;
}
