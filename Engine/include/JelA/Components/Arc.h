#ifndef ARC_H
#define ARC_H
#include "Component.h"
#include "JelA/Geometry.h"

namespace jela
{
    class Arc final : public Component
    {
    public:
        Arc():
            Arc{50,50}
        {}

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

        void SetCloseSegment(bool closeSegment) { m_Arc.SetCloseSegment(closeSegment); }
        void ReplaceArc(float radiusX, float radiusY, float startAngle, float angle)
        {
            m_Arc.SetRadius(radiusX, radiusY);
            m_Arc.SetStartAngle(startAngle);
            m_Arc.SetAngle(angle);
        }
        void SetRadius(const Vector2f& radii) { SetRadius(radii.x, radii.y); }
        void SetRadius(float radiusX, float radiusY)
        {
            m_Arc.SetRadius(radiusX, radiusY);
        }
        void SetStartAngle(float startAngle){ m_Arc.SetStartAngle(startAngle); };
        void SetAngle(float angle){ m_Arc.SetAngle(angle); }
        void SetAngles(float startAngle, float angle)
        {
            m_Arc.SetStartAngle(startAngle);
            m_Arc.SetStartAngle(angle);
        }
        void ReplaceByPoints(const Point2f& point1, const Point2f& point2, bool clockwise)
        {
            m_Arc.SetFromPoints(point1, point2, clockwise);
        }

        void SetOffset(const Point2f& offset) { SetOffset(offset.x, offset.y); }

        void SetOffset(float x, float y) { m_Arc.SetPosition(x, y); };

        void UpdateGeometry() { m_Arc.UpdateGeometry(); }
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
