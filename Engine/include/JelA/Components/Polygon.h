#ifndef POLYGON_H
#define POLYGON_H
#include "Component.h"
#include "JelA/Geometry.h"

namespace jela
{
    class Polygon final : public Component
    {
    public:
        explicit Polygon(const std::vector<Point2f>& points, bool closeSegment = true):
            m_Polygon{std::forward<const std::vector<Point2f>&>(points), closeSegment}
        {};

        void RecreateShape(const std::vector<Point2f>& vertices, bool closeSegment = true) {  m_Polygon.Recreate(vertices, closeSegment); }
        void ResetOffset() { m_Polygon.ResetPosition(); };
        void SetOffset(float x, float y) { SetOffset({ x,y }); }
        void SetOffset(const Vector2f& offset) { m_Polygon.Move(offset - m_Polygon.GetTranslation());}

        std::vector<Point2f> GetOriginalVertices() const { return m_Polygon.GetOriginalPoints(); }
        const std::vector<Point2f>& GetVertices() const { return m_Polygon.GetPoints(); }
        bool IsPointInside(const Point2f& point) const { return m_Polygon.IsPointInside(point); }
        bool IsSegmentClosed() const {return m_Polygon.IsSegmentClosed();}
        const PolygonGeometry& GetPolygon() const { return m_Polygon; }
        Vector2f GetOffset() const { return m_Polygon.GetTranslation(); }

    private:
        PolygonGeometry m_Polygon;
    };
}

#endif //POLYGON_H
