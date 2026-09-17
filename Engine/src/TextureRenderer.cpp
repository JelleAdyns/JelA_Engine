#include "TextureRenderer.h"

#include "Engine.h"
#include "GameObject.h"
#include "TextureImage.h"

namespace jela
{
    void TextureRenderer::Init()
    {
        m_pTextureImage = GetOwner()->GetComponent<TextureImage>();
    }
    void TextureRenderer::Draw() const
    {
        ENGINE.RenderMngr()->DrawTexture(m_pTextureImage->GetRawTexture(),m_pTextureImage->GetDestRect(),Rectf{}, GetOpacity());
    }
    void TextureRenderer::SetOpacity(uint8_t opacityPercentage)
    {
        m_OpacityPercentage = std::min(opacityPercentage, uint8_t{100});
    }
    void TextureRenderer::SetOpacity(float opacity)
    {
        m_OpacityPercentage = static_cast<uint8_t>(std::round(std::min(opacity, 100.f) / 100.f));
    }
    uint8_t TextureRenderer::GetOpacityPercentage() const
    {
        return m_OpacityPercentage;
    }
    float TextureRenderer::GetOpacity() const
    {
        return static_cast<float>(m_OpacityPercentage) / 100.f;
    }
} // jela