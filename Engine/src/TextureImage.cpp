#include "TextureImage.h"

#include "Engine.h"
#include "GameObject.h"
#include "AnchoredRect.h"
#include "Transform.h"

namespace jela
{
    TextureImage::TextureImage(const std::wstring& textureName)
    {
        ENGINE.ResourceMngr()->GetTexture(textureName, m_pTexture);
    }
    void TextureImage::Start()
    {
        m_pAnchoredRect = GetOwner()->GetComponent<AnchoredRect>();
    }
    ResourcePtr<Texture> TextureImage::GetTexture() const
    {
        return m_pTexture;
    }
    const Texture* TextureImage::GetRawTexture() const
    {
        return m_pTexture.get();
    }
    Rectf TextureImage::GetDestRect() const
    {
        if (m_pAnchoredRect == nullptr)
        {
            Vector2f pos = GetOwner()->Transform()->Position();
            Vector2f size = m_pTexture->GetSize();
            Vector2f halfSize = size / 2;

            if constexpr (USE_MATHEMATICAL_COORDINATESYSTEM == false) halfSize.y = -halfSize.y;

            return Rectf{pos.x - halfSize.x, pos.y - halfSize.y, size.x, size.y};
        }

        return m_pAnchoredRect->GetRect();

    }
} // jela