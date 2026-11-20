#ifndef LINERENDERER_H
#define LINERENDERER_H

#include "ShapeRenderers.h"

namespace jela
{
    class LineRenderer final : public OutLineRenderer
    {
    public:

        LineRenderer() = default;
        LineRenderer(const Point2f& point1, const Point2f& point2);

        void Move(const Vector2f& offset);
        void Move(float xOffset, float yOffset);
        void SetPoint1(const Point2f& point);
        void SetPoint1(float x, float y);
        void SetPoint2(const Point2f& point);
        void SetPoint2(float x, float y);

        Point2f GetPoint1() const;
        Point2f GetPoint2() const;

    protected:
        void Line(RenderManager* r, float lineThickness, const StrokeStyle* ss) const override;
    private:
        Point2f m_Point1{};
        Point2f m_Point2{};
    };
}
#endif //LINERENDERER_H
