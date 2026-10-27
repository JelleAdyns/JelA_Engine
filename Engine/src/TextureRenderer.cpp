#include "TextureRenderer.h"

#include "Engine.h"
#include "GameObject.h"
#include "TextureImage.h"
#include "Transform.h"
#include "RectTransform.h"

namespace jela
{
    void TextureRenderer::Start()
    {
        m_pTextureImage = GetOwner()->GetComponent<TextureImage>();
    }
    void TextureRenderer::Draw() const
    {
        auto destRect = m_pTextureImage->GetDestRect();

#ifdef MATHEMATICAL_COORDINATESYSTEM
        destRect.bottom = -destRect.bottom;
#endif

        ENGINE.RenderMngr()->DrawTextureScreenCoord(
            m_pTextureImage->GetTexture(),
            destRect,
            m_pTextureImage->GetSourceRect(),
            GetOpacity());

        if (auto* pRectTranform = GetOwner()->GetComponent<RectTransform>())
        {
            auto pos = -pRectTranform->AnchoredPos();
#ifdef MATHEMATICAL_COORDINATESYSTEM
            pos.y = -pos.y;
#endif
            ENGINE.RenderMngr()->SetColor(RGB(0,255,0));
            ENGINE.RenderMngr()->DrawEllipseScreenCoord( pos.x, pos.y, 20,20);
            ENGINE.RenderMngr()->SetColor(RGB(255,255,255));
        }
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