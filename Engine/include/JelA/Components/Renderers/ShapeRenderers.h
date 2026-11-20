#ifndef SHAPERENDERERS_H
#define SHAPERENDERERS_H

#include "Renderer.h"
#include "JelA/StrokeStyle.h"
#include "JelA/Engine.h"
#include "JelA/GameObject.h"
#include "JelA/Components/LineStylizer.h"

namespace jela
{
    class OutLineRenderer : public Renderer
    {
    public:
        void Start() override{ m_pLineStylizer = GetOwner()->GetComponent<LineStylizer>(); }
        void SetLineThickness(float lineThickness) {m_LineThickness = std::max(1.f,lineThickness);}
        float GetLineThickness() const {return m_LineThickness;}
        void SetLineColor(Color color) { m_LineColor = color; }
        Color GetLineColor() const { return m_LineColor; }

        void Draw() const override
        {
            RenderManager* r = ENGINE.RenderMngr();
            r->SetColor(GetLineColor());
            Line(r, m_LineThickness, m_pLineStylizer ? &m_pLineStylizer->GetStrokeStyle() : nullptr);
        };

    protected:
        LineStylizer* GetLineStylizer() const {return m_pLineStylizer;};
        virtual void Line(RenderManager* r, float lineThickness, const StrokeStyle* ss) const = 0;
    private:
        LineStylizer* m_pLineStylizer{nullptr};
        float m_LineThickness{1.f};
        Color m_LineColor{Color::WHITE};
    };
    template <bool NEVER_USE_OUTLINE = false>
    class FillableRenderer;

    template <>
    class FillableRenderer<false> : public OutLineRenderer
    {
    public:
        void Draw() const override
        {
            if (IsFilled())
            {
                RenderManager* r = ENGINE.RenderMngr();
                r->SetColor(GetFillColor());
                Fill(r);
            }
            if (IsOutlined()) OutLineRenderer::Draw();
        }

        void SetDrawOutline(bool drawOutline) {m_DrawOutline = drawOutline;}
        bool IsOutlined() const {return m_DrawOutline;}
        void SetFill(bool fill) {m_Fill = fill;}
        bool IsFilled() const {return m_Fill;}
        void SetFillColor(Color color) { m_FillColor = color; }
        Color GetFillColor() const { return m_FillColor; }

    protected:
        virtual void Fill(RenderManager* r) const = 0;
        void Line(RenderManager* r, float lineThickness, const StrokeStyle* ss) const override = 0;
    private:
        Color m_FillColor{Color::WHITE};
        bool m_DrawOutline{true};
        bool m_Fill{false};
    };
    template <>
    class FillableRenderer<true> : public Renderer
    {
    public:
        void Draw() const override
        {
            RenderManager* r = ENGINE.RenderMngr();
            r->SetColor(GetFillColor());
            Fill(r);
        };

        void SetFillColor(Color color) { m_FillColor = color; }
        Color GetFillColor() const { return m_FillColor; }
    protected:
        virtual void Fill(RenderManager* r) const = 0;
        virtual void Line(RenderManager*, float, const StrokeStyle*) const {/*NO-OP*/};
    private:
        Color m_FillColor{Color::WHITE};
    };

}

#endif //SHAPERENDERERS_H
