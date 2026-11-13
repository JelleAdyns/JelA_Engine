#ifndef LINERENDERER_H
#define LINERENDERER_H

#include "ShapeRenderers.h"

namespace jela
{
    struct Vector2f;
    using Point2f = Vector2f;

    class LineRenderer final : public ShapeRenderer
    {
    public:

        LineRenderer() = default;
        LineRenderer(const Point2f& point1, const Point2f point2):
            m_Point1{point1}, m_Point2{point2}
        {}

        void Draw() const override;
        void Move(const Vector2f& offset);
        void Move(float xOffset, float yOffset);
        void SetPoint1(const Point2f& point);
        void SetPoint1(float x, float y);
        void SetPoint2(const Point2f& point);
        void SetPoint2(float x, float y);

        Point2f GetPoint1() const;
        Point2f GetPoint2() const;

    private:
        Point2f m_Point1{};
        Point2f m_Point2{};
    };
}
#endif //LINERENDERER_H
