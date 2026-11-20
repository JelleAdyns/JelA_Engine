#ifndef SHAPERENDERER_H
#define SHAPERENDERER_H

#include "ShapeRenderers.h"

namespace jela
{
    template <typename T>
    concept cIsShape = cAnyOf<T, Rectf, RoundedRectf, Ellipsef, Circlef>;

    template <cIsShape Primitive, bool NEVER_USE_OUTLINE = false>
    class PrimitiveRenderer : public FillableRenderer<NEVER_USE_OUTLINE>
    {
    public:
        void SetOffset(const Vector2f& offset)
        {
            if constexpr (Is<Rectf> || Is<RoundedRectf>)
                m_Primitive.SetPos(offset);
            else if constexpr (Is<Ellipsef> || Is<Circlef>)
            {
                m_Primitive.center.x = offset.x;
                m_Primitive.center.y = offset.y;
            }
        }
        void SetWidth(float width)
        {
            width = std::max(1.f, width);

            if constexpr (Is<Rectf> || Is<RoundedRectf>) m_Primitive.width = width;
            else if constexpr (Is<Ellipsef>) m_Primitive.radiusX = width/2;
            else if constexpr (Is<Circlef>) m_Primitive.rad = width/2;
        }
        void SetHeight(float height)
        {
            height = std::max(1.f, height);

            if constexpr (Is<Rectf> || Is<RoundedRectf>) m_Primitive.height = height;
            else if constexpr (Is<Ellipsef>) m_Primitive.radiusY = height/2;
            else if constexpr (Is<Circlef>) m_Primitive.rad = height/2;
        }

        void SetPrimitive(const Primitive& primitive) { m_Primitive = primitive; }

        const Primitive& GetPrimitive() const {return m_Primitive;}
        Primitive& GetPrimitive() {return m_Primitive;}

    protected:
        PrimitiveRenderer() = default;

        void Fill(RenderManager* r) const override
        {
            if constexpr (Is<Rectf>) r->FillRectangle(m_Primitive);
            else if constexpr (Is<RoundedRectf>) r->FillRoundedRect(m_Primitive);
            else if constexpr (Is<Ellipsef>) r->FillEllipse(m_Primitive);
            else if constexpr (Is<Circlef>) r->FillCircle(m_Primitive);
        }
        void Line(RenderManager* r, float lineThickness, const StrokeStyle* ss) const override
        {
            if constexpr (Is<Rectf>) r->DrawRectangle(m_Primitive, lineThickness, ss);
            else if constexpr (Is<RoundedRectf>) r->DrawRoundedRect(m_Primitive, lineThickness, ss);
            else if constexpr (Is<Ellipsef>) r->DrawEllipse(m_Primitive, lineThickness, ss);
            else if constexpr (Is<Circlef>) r->DrawCircle(m_Primitive, lineThickness, ss);
        }

    private:
        template <cIsShape Shape>
        static constexpr bool Is = std::is_same_v<Primitive, Shape>;

        Primitive m_Primitive{};
    };

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    template <bool NEVER_USE_OUTLINE>
    class RectRendererBase : public PrimitiveRenderer<Rectf, NEVER_USE_OUTLINE>{};

    using RectRenderer = RectRendererBase<false>;
    using RectRenderer_DiscardOutline = RectRendererBase<true>;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    template <bool NEVER_USE_OUTLINE>
    class RoundedRectRendererBase : public PrimitiveRenderer<RoundedRectf, NEVER_USE_OUTLINE>
    {
    public:
        void SetRadius(float radiusX, float radiusY)
        {
            PrimitiveRenderer<RoundedRectf, NEVER_USE_OUTLINE>::GetPrimitive().xRadius = std::max(0.f, radiusX);
            PrimitiveRenderer<RoundedRectf, NEVER_USE_OUTLINE>::GetPrimitive().yRadius = std::max(0.f, radiusY);
        }
        void SetRadius(const Vector2f& radii) { SetRadius(radii.x, radii.y); }
        void SetRadius(float radius) { SetRadius(radius, radius); }
    };

    using RoundedRectRenderer = RoundedRectRendererBase<false>;
    using RoundedRectRenderer_DiscardOutline = RoundedRectRendererBase<true>;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    template <bool NEVER_USE_OUTLINE>
    class EllipseRendererBase : public PrimitiveRenderer<Ellipsef, NEVER_USE_OUTLINE>
    {
    public:
        void SetRadius(float radiusX, float radiusY)
        {
            PrimitiveRenderer<Ellipsef, NEVER_USE_OUTLINE>::GetPrimitive().radiusX = std::max(0.f, radiusX);
            PrimitiveRenderer<Ellipsef, NEVER_USE_OUTLINE>::GetPrimitive().radiusY = std::max(0.f, radiusY);
        }
        void SetRadius(const Vector2f& radii) { SetRadius(radii.x, radii.y); }
        void SetRadius(float radius) { SetRadius(radius, radius); }
    };

    using EllipseRenderer = EllipseRendererBase<false>;
    using EllipseRenderer_DiscardOutline = EllipseRendererBase<true>;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    template <bool NEVER_USE_OUTLINE>
    class CircleRendererBase : public PrimitiveRenderer<Circlef, NEVER_USE_OUTLINE>
    {
    public:
        void SetRadius(float radius)
        {
            PrimitiveRenderer<Circlef, NEVER_USE_OUTLINE>::GetPrimitive().rad = std::max(0.f, radius);
        }
    };

    using CircleRenderer = CircleRendererBase<false>;
    using CircleRenderer_DiscardOutline = CircleRendererBase<true>;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
} // jela

#endif //SHAPERENDERER_H
