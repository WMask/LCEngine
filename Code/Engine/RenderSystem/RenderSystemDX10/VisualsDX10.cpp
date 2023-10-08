/**
* VisualsDX10.cpp
* 16.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/VisualsDX10.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "RenderSystem/RenderSystemDX10/TiledVisual2DRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/TextRenderDX10.h"
#include "World/SpriteInterface.h"
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
            texComp->GetTexturePath().c_str(), renderDX10->GetD3D10Device(), &texture, &textureSV, &texSize);
        if (loaded)
            texComp->SetTextureSize(ToF(texSize));
        else
            throw std::exception("LcSpriteDX10::AddComponent(): Cannot load texture");
    }

    LC_CATCH{ LC_THROW("LcSpriteDX10::AddComponent()") }
}

void LcWidgetDX10::Update(float deltaSeconds, const LcAppContext& context)
{
    IVisualBase::Update(deltaSeconds, context);

    auto textComp = GetTextComponent();
    if (textComp && textComp->GetText() != prevRenderedText)
    {
        auto renderDX10 = static_cast<LcRenderSystemDX10*>(context.render);
        auto textRender = renderDX10 ? renderDX10->GetTextRender() : nullptr;
        if (textRender && font)
        {
            auto size = GetSize() * context.world->GetWorldScale().GetScale();
            LcRectf rect{ 0.0f, 0.0f, size.x, size.y };
            textRender->RenderText(textComp->GetText(), rect, textComp->GetSettings().textColor,
                textComp->GetSettings().textAlign, font, textRenderTarget.Get(), context);
            prevRenderedText = textComp->GetText();
        }
    }
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
        bool loaded = textureLoader->LoadTexture(texComp->GetTexturePath().c_str(),
            renderDX10->GetD3D10Device(), &spriteTexture, &spriteTextureSV, &texSize);
        if (loaded)
            texComp->SetTextureSize(ToF(texSize));
        else
            throw std::exception("LcWidgetDX10::AddComponent(): Cannot load texture");
    }

    auto textComp = GetTextComponent();
    if (textComp)
    {
        auto textRender = renderDX10 ? renderDX10->GetTextRender() : nullptr;
        if (!textRender) throw std::exception("LcWidgetDX10::AddComponent(): Invalid widget render");

        font = textRender->AddFont(textComp->GetSettings().fontName, GetFontSize(*textComp, context), textComp->GetSettings().fontWeight);
        if (!font) throw std::exception("LcWidgetDX10::AddComponent(): Cannot create font");

        RedrawText(textRender, context);
    }

    LC_CATCH{ LC_THROW("LcWidgetDX10::AddComponent()") }
}

void LcWidgetDX10::RecreateFont(const LcAppContext& context)
{
    auto textComp = GetTextComponent();
    if (auto textComp = GetTextComponent())
    {
        auto renderDX10 = static_cast<LcRenderSystemDX10*>(context.render);
        auto textRender = renderDX10 ? renderDX10->GetTextRender() : nullptr;
        if (!textRender) throw std::exception("LcWidgetDX10::RecreateFont(): Invalid widget render");

        font = textRender->AddFont(textComp->GetSettings().fontName, GetFontSize(*textComp, context), textComp->GetSettings().fontWeight);
        if (!font) throw std::exception("LcWidgetDX10::RecreateFont(): Cannot create font");

        RedrawText(textRender, context);
    }
}

void LcWidgetDX10::RedrawText(LcTextRenderDX10* textRender, const LcAppContext& context)
{
    if (auto textComp = GetTextComponent())
    {
        textRenderTarget.Reset();
        textTextureSV.Reset();
        textTexture.Reset();

        textRender->CreateTextureAndRenderTarget(*this, context.world->GetWorldScale().GetScale(), context);

        auto size = GetSize() * context.world->GetWorldScale().GetScale();
        LcRectf rect{ 0.0f, 0.0f, size.x, size.y };
        textRender->RenderText(textComp->GetText(), rect, textComp->GetSettings().textColor,
            textComp->GetSettings().textAlign, font, textRenderTarget.Get(), context);
        prevRenderedText = textComp->GetText();
    }
}

float LcWidgetDX10::GetFontSize(const IWidgetTextComponent& textComp, const LcAppContext& context) const
{
    float scale = context.world->GetWorldScale().GetScaleFonts() ? context.world->GetWorldScale().GetScale().y : 1.0f;
    return static_cast<float>(textComp.GetSettings().fontSize) * scale;
}
