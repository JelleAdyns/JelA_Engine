#include "JelA/Components/Renderers/PrimitiveRenderers.h"

#include "JelA/Engine.h"

namespace jela
{
    RectfRenderer::RectfRenderer(const Rectf& rect): PrimitiveRenderer{std::forward<const Rectf&>(rect)}
    {}
    void RectfRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();
        r->SetColor(GetColor());
        if (IsFilled()) r->FillRectangle(GetPrimitive());
        else r->DrawRectangle(GetPrimitive(), GetLineThickness(), &GetStrokeStyle());
    }

    RoundedRectfRenderer::RoundedRectfRenderer(const RoundedRectf& rect): PrimitiveRenderer{std::forward<const RoundedRectf&>(rect)}
    {}
    void RoundedRectfRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();
        r->SetColor(GetColor());
        if (IsFilled()) r->FillRoundedRect(GetPrimitive());
        else r->DrawRoundedRect(GetPrimitive(), GetLineThickness(), &GetStrokeStyle());
    }
    void RoundedRectfRenderer::SetRadius(float radiusX, float radiusY)
    {
        GetPrimitive().xRadius = std::max(0.f, radiusX);
        GetPrimitive().yRadius = std::max(0.f, radiusY);
    }
    void RoundedRectfRenderer::SetRadius(const Vector2f& radii){SetRadius(radii.x, radii.y);}
    void RoundedRectfRenderer::SetRadius(float radius){ SetRadius(radius, radius); }

    EllipsefRenderer::EllipsefRenderer(const Ellipsef& ellipse): PrimitiveRenderer{std::forward<const Ellipsef&>(ellipse)}
    {}
    void EllipsefRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();
        r->SetColor(GetColor());
        if (IsFilled()) r->FillEllipse(GetPrimitive());
        else r->DrawEllipse(GetPrimitive(), GetLineThickness(), &GetStrokeStyle());
    }
    void EllipsefRenderer::SetRadius(float radiusX, float radiusY)
    {
        GetPrimitive().radiusX = std::max(0.f, radiusX);
        GetPrimitive().radiusY = std::max(0.f, radiusY);
    }
    void EllipsefRenderer::SetRadius(const Vector2f& radii){SetRadius(radii.x, radii.y);}
    void EllipsefRenderer::SetRadius(float radius){ SetRadius(radius, radius); }

    CirclefRenderer::CirclefRenderer(const Circlef& circle): PrimitiveRenderer{std::forward<const Circlef&>(circle)}
    {}
    void CirclefRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();
        r->SetColor(GetColor());
        if (IsFilled()) r->FillCircle(GetPrimitive());
        else r->DrawCircle(GetPrimitive(), GetLineThickness(), &GetStrokeStyle());
    }
    void CirclefRenderer::SetRadius(float radius){ GetPrimitive().rad = std::max(0.f, radius); }
}
