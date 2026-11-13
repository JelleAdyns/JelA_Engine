#ifndef SHAPERENDERER_H
#define SHAPERENDERER_H

#include "ShapeRenderers.h"

namespace jela
{

    struct Rectf;
    struct RoundedRectf;
    struct Ellipsef;
    struct Circlef;
    struct Vector2f;

    class RectfRenderer final : public PrimitiveRenderer<Rectf>
    {
    public:
        RectfRenderer() = default;
        explicit RectfRenderer(const Rectf& rect);
        void Draw() const override;
    };
    class RoundedRectfRenderer final : public PrimitiveRenderer<RoundedRectf>
    {
    public:
        RoundedRectfRenderer() = default;
        explicit RoundedRectfRenderer(const RoundedRectf& rect);
        void Draw() const override;
        void SetRadius(float radiusX, float radiusY);
        void SetRadius(const Vector2f& radii);
        void SetRadius(float radius);
    };
    class EllipsefRenderer final : public PrimitiveRenderer<Ellipsef>
    {
    public:
        EllipsefRenderer() = default;
        explicit EllipsefRenderer(const Ellipsef& ellipse);
        void Draw() const override;
        void SetRadius(float radiusX, float radiusY);
        void SetRadius(const Vector2f& radii);
        void SetRadius(float radius);
    };
    class CirclefRenderer final : public PrimitiveRenderer<Circlef>
    {
    public:
        CirclefRenderer() = default;
        explicit CirclefRenderer(const Circlef& circle);
        void Draw() const override;
        void SetRadius(float radius);
    };

} // jela

#endif //SHAPERENDERER_H
