/**
* VisualsVulkan.cpp
* 29.08.2024
* (c) Denis Romakhov
*/

#include "RenderSystem/RenderSystemVulkan/VisualsVulkan.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"
#include "World/SpriteInterface.h"
#include "Core/LCLocalization.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"


void LcSpriteVulkan::AddComponent(TVComponentPtr comp, const LcAppContext& context)
{
    LC_TRY

    LcSprite::AddComponent(comp, context);

    auto renderVulkan = static_cast<LcRenderSystemVulkan*>(context.render);
    auto texComp = GetTextureComponent();
    if (texComp && renderVulkan)
    {
        LcTextureVulkan texture{};
        renderVulkan->GetTextureLoader().LoadTexture(texComp->GetTexturePath(), texture);
        texComp->SetTextureSize(ToF(texture.size));
    }

    LC_CATCH{ LC_THROW("LcSpriteVulkan::AddComponent()") }
}

void LcWidgetVulkan::AddComponent(TVComponentPtr comp, const LcAppContext& context)
{
    LC_TRY

    LcWidget::AddComponent(comp, context);

    auto renderVulkan = static_cast<LcRenderSystemVulkan*>(context.render);
    auto texComp = GetTextureComponent();
    if (texComp && renderVulkan)
    {
        LcTextureVulkan texture{};
        renderVulkan->GetTextureLoader().LoadTexture(texComp->GetTexturePath(), texture);
        texComp->SetTextureSize(ToF(texture.size));
    }

    LC_CATCH{ LC_THROW("LcWidgetVulkan::AddComponent()") }
}
