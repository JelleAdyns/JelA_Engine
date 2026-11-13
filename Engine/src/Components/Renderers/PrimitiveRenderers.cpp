#include "JelA/Components/Renderers/PrimitiveRenderers.h"

#include "JelA/Engine.h"

namespace jela
{
    RectfRenderer::RectfRenderer(const Rectf& rect): PrimitiveRenderer{std::forward<const Rectf&>(rect)}
    {}
    void RectfRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();
        r->SetColor(m_Color);
        if (m_Fill) r->FillRectangle(m_Primitive);
        else r->DrawRectangle(m_Primitive, m_LineThickness);
    }

    RoundedRectfRenderer::RoundedRectfRenderer(const RoundedRectf& rect): PrimitiveRenderer{std::forward<const RoundedRectf&>(rect)}
    {}
    void RoundedRectfRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();
        r->SetColor(m_Color);
        if (m_Fill) r->FillRoundedRect(m_Primitive);
        else r->DrawRoundedRect(m_Primitive, m_LineThickness);
    }
    void RoundedRectfRenderer::SetRadius(float radiusX, float radiusY)
    {
        m_Primitive.xRadius = std::max(0.f, radiusX);
        m_Primitive.yRadius = std::max(0.f, radiusY);
    }
    void RoundedRectfRenderer::SetRadius(const Vector2f& radii){SetRadius(radii.x, radii.y);}
    void RoundedRectfRenderer::SetRadius(float radius){ SetRadius(radius, radius); }

    EllipsefRenderer::EllipsefRenderer(const Ellipsef& ellipse): PrimitiveRenderer{std::forward<const Ellipsef&>(ellipse)}
    {}
    void EllipsefRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();
        r->SetColor(m_Color);
        if (m_Fill) r->FillEllipse(m_Primitive);
        else r->DrawEllipse(m_Primitive, m_LineThickness);
    }
    void EllipsefRenderer::SetRadius(float radiusX, float radiusY)
    {
        m_Primitive.radiusX = std::max(0.f, radiusX);
        m_Primitive.radiusY = std::max(0.f, radiusY);
    }
    void EllipsefRenderer::SetRadius(const Vector2f& radii){SetRadius(radii.x, radii.y);}
    void EllipsefRenderer::SetRadius(float radius){ SetRadius(radius, radius); }

    CirclefRenderer::CirclefRenderer(const Circlef& circle): PrimitiveRenderer{std::forward<const Circlef&>(circle)}
    {}
    void CirclefRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();
        r->SetColor(m_Color);
        if (m_Fill) r->FillCircle(m_Primitive);
        else r->DrawCircle(m_Primitive, m_LineThickness);
    }
    void CirclefRenderer::SetRadius(float radius){ m_Primitive.rad = std::max(0.f, radius); }
}
