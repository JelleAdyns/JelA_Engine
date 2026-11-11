#ifndef LINERENDERER_H
#define LINERENDERER_H

#include "Engine.h"
#include "Renderers.h"

namespace jela
{
    class LineRenderer final : public ShapeRenderer
    {
    public:

        LineRenderer() = default;
        LineRenderer(const Point2f& point1, const Point2f point2):
            m_Point1{point1}, m_Point2{point2}
        {}

        void Draw() const override
        {
            const auto r = ENGINE.RenderMngr();
            r->SetColor(static_cast<COLORREF>(m_Color), m_Color.a);
            r->DrawLine(m_Point1, m_Point2, m_LineThickness);
        }
        void Move(const Vector2f& offset) {Move(offset.x,offset.y);}
        void Move(float xOffset, float yOffset) { m_Point1.x += xOffset; m_Point1.y += yOffset; m_Point2.x += xOffset; m_Point2.y += yOffset; }
        void SetPoint1(const Point2f& point) {SetPoint1(point.x,point.y);}
        void SetPoint1(float x, float y) { m_Point1.x = x; m_Point1.y = y; }
        void SetPoint2(const Point2f& point) {SetPoint2(point.x,point.y);}
        void SetPoint2(float x, float y) { m_Point2.x = x; m_Point2.y = y; }

        Point2f GetPoint1() const {return m_Point1;}
        Point2f GetPoint2() const {return m_Point2;}

    private:
        Point2f m_Point1{};
        Point2f m_Point2{};
    };
}
#endif //LINERENDERER_H
