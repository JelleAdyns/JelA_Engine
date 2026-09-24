#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H
#include "Renderer.h"
#include "ResourceManager.h"


namespace jela
{
    class RectTransform;

    class TextRenderer : public Renderer
    {
    public:
        void Start() override;
        void Draw() const override;

        void SetDrawRect(bool drawRect);
        void SetFont(const tstring& fontName, bool fromFile);
        void SetText(const tstring& text);
        void SetSize(float fontSize);
        void SetBold(bool bold);
        void SetItalic(bool italic);
        void SetStyle(bool bold, bool italic);
        void SetHorizontalAllignment(TextFormat::HorAllignment horAllignment);
        void SetVerticalAllignment(TextFormat::VertAllignment vertAllignment);
        void SetAlignment(TextFormat::HorAllignment horAllignment, TextFormat::VertAllignment vertAllignment);
        void SetTextFormat(float fontSize, bool bold, bool italic, TextFormat::HorAllignment horAllignment, TextFormat::VertAllignment vertAllignment);
    private:
        tstring m_Text{};
        ResourcePtr<Font> m_pFont{};
        std::unique_ptr<TextFormat> m_pTextFormat{};
        RectTransform* m_pRectTransform{};
        bool m_DrawRect{false};
    };
}



#endif //TEXTRENDERER_H
