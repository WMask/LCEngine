/**
* VisualsDX10.cpp
* 16.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/VisualsDX10.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "RenderSystem/RenderSystemDX10/TiledVisual2DRenderDX10.h"
#include "World/Sprites.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"

#include <cmath>


void LcSpriteDX10::Destroy(const LcAppContext& context)
{
    auto renderDX10 = static_cast<LcRenderSystemDX10*>(context.render);
    if (auto tiledRender = renderDX10 ? renderDX10->GetTiledRender() : nullptr)
    {
        tiledRender->RemoveTiles(this);
    }
}

void LcSpriteDX10::AddComponent(TVComponentPtr comp, const LcAppContext& context)
{
    LC_TRY

        LcSprite::AddComponent(comp, context);

    auto renderDX10 = static_cast<LcRenderSystemDX10*>(context.render);
    auto texComp = GetTextureComponent();
    if (texComp && renderDX10)
    {
        LcSize texSize;
        bool loaded = renderDX10->GetTextureLoader()->LoadTexture(
            texComp->texture.c_str(), renderDX10->GetD3D10Device(), texture.GetAddressOf(), shaderView.GetAddressOf(), &texSize);
        if (loaded)
            texComp->texSize = ToF(texSize);
        else
            throw std::exception("LcSpriteDX10::AddComponent(): Cannot load texture");
    }

    LC_CATCH{ LC_THROW("LcSpriteDX10::AddComponent()") }
}

void LcWidgetDX10::AddComponent(TVComponentPtr comp, const LcAppContext& context)
{
    LC_TRY

    LcWidget::AddComponent(comp, context);

    auto renderDX10 = static_cast<LcRenderSystemDX10*>(context.render);
    auto textureLoader = renderDX10 ? renderDX10->GetTextureLoader() : nullptr;
    if (!renderDX10 || !textureLoader) throw std::exception("LcWidgetDX10::AddComponent(): Invalid render system");

    if (auto texComp = GetTextureComponent())
    {
        LcSize texSize;
        bool loaded = textureLoader->LoadTexture(texComp->texture.c_str(),
            renderDX10->GetD3D10Device(), &spriteTexture, &spriteTextureSV, &texSize);
        if (loaded)
            texComp->texSize = ToF(texSize);
        else
            throw std::exception("LcWidgetDX10::AddComponent(): Cannot load texture");
    }

    auto textComp = GetTextComponent();
    if (textComp)
    {
        auto d3dDevice = renderDX10 ? renderDX10->GetD3D10Device() : nullptr;
        auto widgetRender = renderDX10 ? renderDX10->GetWidgetRender() : nullptr;
        if (!d3dDevice || !widgetRender) throw std::exception("LcWidgetDX10::AddComponent(): Invalid render system");

        font = widgetRender->AddFont(textComp->fontName, GetFontSize(*textComp, context), textComp->fontWeight);
        if (!font) throw std::exception("LcWidgetDX10::AddComponent(): Cannot create font");

        RedrawText(d3dDevice, widgetRender, context);
    }

    LC_CATCH{ LC_THROW("LcWidgetDX10::AddComponent()") }
}

void LcWidgetDX10::RecreateFont(const LcAppContext& context)
{
    auto textComp = GetTextComponent();
    if (auto textComp = GetTextComponent())
    {
        auto renderDX10 = static_cast<LcRenderSystemDX10*>(context.render);
        auto d3dDevice = renderDX10 ? renderDX10->GetD3D10Device() : nullptr;
        auto widgetRender = renderDX10 ? renderDX10->GetWidgetRender() : nullptr;
        if (!d3dDevice || !widgetRender) throw std::exception("LcWidgetDX10::RecreateFont(): Invalid render system");

        font = widgetRender->AddFont(textComp->fontName, GetFontSize(*textComp, context), textComp->fontWeight);
        if (!font) throw std::exception("LcWidgetDX10::RecreateFont(): Cannot create font");

        RedrawText(d3dDevice, widgetRender, context);
    }
}

void LcWidgetDX10::RedrawText(ID3D10Device1* d3dDevice, LcWidgetRenderDX10* widgetRender, const LcAppContext& context)
{
    if (auto textComp = GetTextComponent())
    {
        textRenderTarget.Reset();
        textTextureSV.Reset();
        textTexture.Reset();

        widgetRender->CreateTextureAndRenderTarget(*this, context.world.GetWorldScale().scale, context);

        auto size = GetSize() * context.world.GetWorldScale().scale;
        LcRectf rect{ 0.0f, 0.0f, size.x, size.y };
        widgetRender->RenderText(textComp->text, rect, textComp->textColor, font, textRenderTarget.Get(), context);
    }
}

float LcWidgetDX10::GetFontSize(const LcWidgetTextComponent& textComp, const LcAppContext& context) const
{
    float scale = context.world.GetWorldScale().scaleFonts ? context.world.GetWorldScale().scale.y : 1.0f;
    return static_cast<float>(textComp.fontSize) * scale;
}
