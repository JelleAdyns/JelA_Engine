#include "TextRenderer.h"

#include "Engine.h"
#include "GameObject.h"
#include "RectTransform.h"

namespace jela
{
    void TextRenderer::Start()
    {
        m_pRectTransform = GetOwner()->GetComponent<RectTransform>();
    }
    void TextRenderer::Draw() const
    {
        ENGINE.ResourceMngr()->SetCurrentFont(m_pFont.get());
        ENGINE.ResourceMngr()->SetCurrentTextFormat(m_pTextFormat.get());
        if (m_pRectTransform)
        {
            ENGINE.RenderMngr()->DrawString(m_Text, m_pRectTransform->GetWorldRect(), m_DrawRect);
            return;
        }

        const Vector2f worldPos = GetOwner()->Transform()->WorldPosition();
        const Vector2f size
        {
            m_pTextFormat->GetFontSize() * (m_Text.size() +2),
            m_pTextFormat->GetFontSize()
        };
        const Vector2f center
        {
            worldPos.x - size.x/2,
            worldPos.y - size.y/2
        };
        ENGINE.RenderMngr()->DrawString(m_Text, Rectf{center,size.x, size.y}, m_DrawRect);
    }
    void TextRenderer::SetDrawRect(bool drawRect)
    {
        m_DrawRect = drawRect;
    }
    void TextRenderer::SetFont(const std::wstring& fontName, bool fromFile)
    {
        ENGINE.ResourceMngr()->GetFont(fontName, m_pFont, fromFile);
    }
    void TextRenderer::SetText(const tstring& text)
    {
        m_Text = text;
    }
    void TextRenderer::SetSize(float fontSize)
    {
        m_pTextFormat = std::make_unique<TextFormat>(
            fontSize,
            m_pTextFormat->IsBold(),
            m_pTextFormat->IsItalic(),
            m_pTextFormat->HorizontalAllignment(),
            m_pTextFormat->VerticalAllignment());
    }
    void TextRenderer::SetBold(bool bold)
    {
        m_pTextFormat = std::make_unique<TextFormat>(
            m_pTextFormat->GetFontSize(),
            bold,
            m_pTextFormat->IsItalic(),
            m_pTextFormat->HorizontalAllignment(),
            m_pTextFormat->VerticalAllignment());

    }
    void TextRenderer::SetItalic(bool italic)
    {
        m_pTextFormat = std::make_unique<TextFormat>(
            m_pTextFormat->GetFontSize(),
            m_pTextFormat->IsBold(),
            italic,
            m_pTextFormat->HorizontalAllignment(),
            m_pTextFormat->VerticalAllignment());

    }
    void TextRenderer::SetStyle(bool bold, bool italic)
    {
        m_pTextFormat = std::make_unique<TextFormat>(
            m_pTextFormat->GetFontSize(),
            bold,
            italic,
            m_pTextFormat->HorizontalAllignment(),
            m_pTextFormat->VerticalAllignment());

    }
    void TextRenderer::SetHorizontalAllignment(TextFormat::HorAllignment horAllignment)
    {
        m_pTextFormat = std::make_unique<TextFormat>(
            m_pTextFormat->GetFontSize(),
            m_pTextFormat->IsBold(),
            m_pTextFormat->IsItalic(),
            horAllignment,
            m_pTextFormat->VerticalAllignment());

    }
    void TextRenderer::SetVerticalAllignment(TextFormat::VertAllignment vertAllignment)
    {
        m_pTextFormat = std::make_unique<TextFormat>(
            m_pTextFormat->GetFontSize(),
            m_pTextFormat->IsBold(),
            m_pTextFormat->IsItalic(),
            m_pTextFormat->HorizontalAllignment(),
            vertAllignment);

    }
    void TextRenderer::SetAlignment(TextFormat::HorAllignment horAllignment, TextFormat::VertAllignment vertAllignment)
    {
        m_pTextFormat = std::make_unique<TextFormat>(
             m_pTextFormat->GetFontSize(),
             m_pTextFormat->IsBold(),
             m_pTextFormat->IsItalic(),
             horAllignment,
             vertAllignment);
    }
    void TextRenderer::SetTextFormat(float fontSize, bool bold, bool italic, TextFormat::HorAllignment horAllignment, TextFormat::VertAllignment vertAllignment)
    {
        m_pTextFormat = std::make_unique<TextFormat>(fontSize, bold, italic, horAllignment, vertAllignment);
    }
}
