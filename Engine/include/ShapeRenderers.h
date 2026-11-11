#ifndef SHAPERENDERER_H
#define SHAPERENDERER_H

#include "Engine.h"
#include "Renderer.h"
#include "Structs.h"

namespace jela
{
    template <typename T, typename... Types>
    concept cAnyOf = std::disjunction_v<std::is_same<T, Types>...>;

    template <typename T>
    concept cIsShape = cAnyOf<T, Rectf, RoundedRectf, Ellipsef, Circlef>;

    template <cIsShape Shape>
    class ShapeRenderer : public Renderer
    {
    public:

        ShapeRenderer() = default;
        explicit ShapeRenderer(const Shape& shape): m_Shape{shape} {}

        void SetOffset(const Vector2f& offset)
        {
            if constexpr (std::is_same_v<Shape, Rectf> || std::is_same_v<Shape, RoundedRectf>)
                m_Shape.SetPos(offset);
            else if constexpr (std::is_same_v<Shape, Ellipsef> || std::is_same_v<Shape, Circlef>)
            {
                m_Shape.center.x = offset.x;
                m_Shape.center.y = offset.y;
            }
        }
        void SetWidth(float width)
        {
            width = std::max(1.f, width);

            if constexpr (std::is_same_v<Shape, Rectf> || std::is_same_v<Shape, RoundedRectf>) m_Shape.width = width;
            else if constexpr (std::is_same_v<Shape, Ellipsef>) m_Shape.radiusX = width/2;
            else if constexpr (std::is_same_v<Shape, Circlef>) m_Shape.rad = width/2;
        }
        void SetHeight(float height)
        {
            height = std::max(1.f, height);

            if constexpr (std::is_same_v<Shape, Rectf> || std::is_same_v<Shape, RoundedRectf>) m_Shape.height = height;
            else if constexpr (std::is_same_v<Shape, Ellipsef>) m_Shape.radiusY = height/2;
            else if constexpr (std::is_same_v<Shape, Circlef>) m_Shape.rad = height/2;
        }

        void SetShape(const Shape& shape) const {m_Shape = shape;}
        void SetLineThickness(float lineThickness) {m_LineThickness = std::max(1.f,lineThickness);}
        void SetFill(bool fill) {m_Fill = fill;}

        const Shape& GetShape() const {return m_Shape;}
        float GetLineThickness() const {return m_LineThickness;}
        bool IsFilled() const {return m_Fill;}

        void Draw() const override = 0;
    protected:
        Shape m_Shape{};
        float m_LineThickness{1.f};
        bool m_Fill{false};
    };

    class RectfRenderer final : public ShapeRenderer<Rectf>
    {
    public:
        RectfRenderer() = default;
        explicit RectfRenderer(const Rectf& rect) : ShapeRenderer{std::forward<const Rectf&>(rect)} {}
        void Draw() const override
        {
            if (m_Fill) ENGINE.RenderMngr()->FillRectangle(m_Shape);
            else ENGINE.RenderMngr()->DrawRectangle(m_Shape, m_LineThickness);
        }
    };
    class RoundedRectfRenderer final : public ShapeRenderer<RoundedRectf>
    {
    public:
        RoundedRectfRenderer() = default;
        explicit RoundedRectfRenderer(const RoundedRectf& rect) : ShapeRenderer{std::forward<const RoundedRectf&>(rect)} {}
        void Draw() const override
        {
            if (m_Fill) ENGINE.RenderMngr()->FillRoundedRect(m_Shape);
            else ENGINE.RenderMngr()->DrawRoundedRect(m_Shape, m_LineThickness);
        }
        void SetRadius(float radiusX, float radiusY)
        {
            m_Shape.xRadius = std::max(0.f, radiusX);
            m_Shape.yRadius = std::max(0.f, radiusY);
        }
        void SetRadius(const Vector2f& radii){SetRadius(radii.x, radii.y);}
        void SetRadius(float radius) { SetRadius(radius, radius); }
    };
    class EllipsefRenderer final : public ShapeRenderer<Ellipsef>
    {
    public:
        EllipsefRenderer() = default;
        explicit EllipsefRenderer(const Ellipsef& ellipse) : ShapeRenderer{std::forward<const Ellipsef&>(ellipse)} {}
        void Draw() const override
        {
            if (m_Fill) ENGINE.RenderMngr()->FillEllipse(m_Shape);
            else ENGINE.RenderMngr()->DrawEllipse(m_Shape, m_LineThickness);
        }
        void SetRadius(float radiusX, float radiusY)
        {
            m_Shape.radiusX = std::max(0.f, radiusX);
            m_Shape.radiusY = std::max(0.f, radiusY);
        }
        void SetRadius(const Vector2f& radii){SetRadius(radii.x, radii.y);}
        void SetRadius(float radius) { SetRadius(radius, radius); }
    };
    class CirclefRenderer final : public ShapeRenderer<Circlef>
    {
    public:
        CirclefRenderer() = default;
        explicit CirclefRenderer(const Circlef& circle) : ShapeRenderer{std::forward<const Circlef&>(circle)} {}
        void Draw() const override
        {
            if (m_Fill) ENGINE.RenderMngr()->FillCircle(m_Shape);
            else ENGINE.RenderMngr()->DrawCircle(m_Shape, m_LineThickness);
        }
        void SetRadius(float radius) { m_Shape.rad = std::max(0.f, radius); }
    };

} // jela

#endif //SHAPERENDERER_H
