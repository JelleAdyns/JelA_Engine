#include "TextureImage.h"

#include "Engine.h"
#include "GameObject.h"
#include "RectTransform.h"
#include "Transform.h"

namespace jela
{
    TextureImage::TextureImage(const std::wstring& textureName)
    {
        ENGINE.ResourceMngr()->GetTexture(textureName, m_pTexture);
    }
    void TextureImage::Start()
    {
        m_pRectTransform = GetOwner()->GetComponent<RectTransform>();
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
        if (m_pRectTransform) return m_pRectTransform->GetRect();

        const Vector2f pos = GetOwner()->Transform()->Position();
        const Vector2f size = m_pTexture->GetSize();

        return Rectf{pos.x - size.x / 2, pos.y - size.y / 2, size.x, size.y};
    }
} // jela