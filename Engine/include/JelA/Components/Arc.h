#ifndef ARC_H
#define ARC_H
#include "Component.h"
#include "JelA/Geometry.h"

namespace jela
{
    class Arc final : public Component
    {
    public:
        Arc(float radiusX, float radiusY, float angle = 90.f, bool closeSegment = true):
            m_Arc{0.f, 0.f, radiusX, radiusY, 0.f, angle, closeSegment}
        {}
        Arc(float centerX, float centerY, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment):
            m_Arc{centerX, centerY, radiusX, radiusY, startAngle, angle, closeSegment}
        {}
        Arc(const Point2f& center, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment):
            Arc{center.x, center.y, radiusX, radiusY, startAngle, angle, closeSegment}
        {}
        Arc(const Point2f& point1, const Point2f& point2, bool clockwise, bool closeSegment):
            m_Arc{point1, point2, clockwise, closeSegment}
        {}

        bool Recreate(float radiusX, float radiusY, float startAngle, float angle, bool closeSegment);
        bool RecreateByRadius(float radiusX, float radiusY, bool closeSegment);
        bool RecreateByAngle(float startAngle, float angle, bool closeSegment);
        bool Recreate(const Point2f& point1, const Point2f& point2, bool clockwise, bool closeSegment);

        void SetOffset(const Point2f& offset) { SetOffset(offset.x, offset.y); }

        void SetOffset(float x, float y) { m_Arc.SetPosition(x, y); };

        Vector2f GetOffset() const { return m_Arc.GetTranslation(); }
        const Vector2f& GetRadius() const { return m_Arc.GetRadius(); }
        float GetStartAngle() const { return m_Arc.GetStartAngle(); }
        float GetAngle() const { return m_Arc.GetAngle(); }
        const ArcGeometry& GetArc() const { return m_Arc; }
    private:
        ArcGeometry m_Arc;
    };
}

#endif //ARC_H
