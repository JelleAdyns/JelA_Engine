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
    Rectf TextureImage::GetSourceRect() const
    {
        return m_SourceRect;
    }
    void TextureImage::SetSourceRectPos(float x, float y)
    {
        m_SourceRect.left = x;
#ifdef MATHEMATICAL_COORDINATESYSTEM
        m_SourceRect.bottom = y;
#else
        m_SourceRect.top = y;
#endif
    }
    void TextureImage::SetSourceRectPos(Vector2f newPos)
    {
        SetSourceRectPos(newPos.x, newPos.y);
    }
    void TextureImage::SetSourceRectSize(float width, float height)
    {
        m_SourceRect.width = width;
        m_SourceRect.height = height;
    }
    void TextureImage::SetSourceRectSize(Vector2f size)
    {
        SetSourceRectSize(size.x, size.y);
    }
    void TextureImage::SetSourceRect(Rectf rect)
    {
        m_SourceRect = rect;
    }
} // jela