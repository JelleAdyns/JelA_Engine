#ifndef SHAPERENDERER_H
#define SHAPERENDERER_H

#include "Engine.h"
#include "Renderers.h"
#include "Structs.h"

namespace jela
{

    class RectfRenderer final : public PrimitiveRenderer<Rectf>
    {
    public:
        RectfRenderer() = default;
        explicit RectfRenderer(const Rectf& rect) : PrimitiveRenderer{std::forward<const Rectf&>(rect)} {}
        void Draw() const override
        {
            const auto r = ENGINE.RenderMngr();
            r->SetColor(static_cast<COLORREF>(m_Color), m_Color.a);
            if (m_Fill) r->FillRectangle(m_Primitive);
            else r->DrawRectangle(m_Primitive, m_LineThickness);
        }
    };
    class RoundedRectfRenderer final : public PrimitiveRenderer<RoundedRectf>
    {
    public:
        RoundedRectfRenderer() = default;
        explicit RoundedRectfRenderer(const RoundedRectf& rect) : PrimitiveRenderer{std::forward<const RoundedRectf&>(rect)} {}
        void Draw() const override
        {
            const auto r = ENGINE.RenderMngr();
            r->SetColor(static_cast<COLORREF>(m_Color), m_Color.a);
            if (m_Fill) r->FillRoundedRect(m_Primitive);
            else r->DrawRoundedRect(m_Primitive, m_LineThickness);
        }
        void SetRadius(float radiusX, float radiusY)
        {
            m_Primitive.xRadius = std::max(0.f, radiusX);
            m_Primitive.yRadius = std::max(0.f, radiusY);
        }
        void SetRadius(const Vector2f& radii){SetRadius(radii.x, radii.y);}
        void SetRadius(float radius) { SetRadius(radius, radius); }
    };
    class EllipsefRenderer final : public PrimitiveRenderer<Ellipsef>
    {
    public:
        EllipsefRenderer() = default;
        explicit EllipsefRenderer(const Ellipsef& ellipse) : PrimitiveRenderer{std::forward<const Ellipsef&>(ellipse)} {}
        void Draw() const override
        {
            const auto r = ENGINE.RenderMngr();
            r->SetColor(static_cast<COLORREF>(m_Color), m_Color.a);
            if (m_Fill) r->FillEllipse(m_Primitive);
            else r->DrawEllipse(m_Primitive, m_LineThickness);
        }
        void SetRadius(float radiusX, float radiusY)
        {
            m_Primitive.radiusX = std::max(0.f, radiusX);
            m_Primitive.radiusY = std::max(0.f, radiusY);
        }
        void SetRadius(const Vector2f& radii){SetRadius(radii.x, radii.y);}
        void SetRadius(float radius) { SetRadius(radius, radius); }
    };
    class CirclefRenderer final : public PrimitiveRenderer<Circlef>
    {
    public:
        CirclefRenderer() = default;
        explicit CirclefRenderer(const Circlef& circle) : PrimitiveRenderer{std::forward<const Circlef&>(circle)} {}
        void Draw() const override
        {
            const auto r = ENGINE.RenderMngr();
            r->SetColor(static_cast<COLORREF>(m_Color), m_Color.a);
            if (m_Fill) r->FillCircle(m_Primitive);
            else r->DrawCircle(m_Primitive, m_LineThickness);
        }
        void SetRadius(float radius) { m_Primitive.rad = std::max(0.f, radius); }
    };

} // jela

#endif //SHAPERENDERER_H
