#include "JelA/Components/Renderers/LineRenderer.h"
#include "JelA/Engine.h"

namespace jela
{
    void LineRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();
        r->SetColor(static_cast<COLORREF>(m_Color), m_Color.a);
        r->DrawLine(m_Point1, m_Point2, m_LineThickness);
    }
    void LineRenderer::Move(const Vector2f& offset){Move(offset.x,offset.y);}
    void LineRenderer::Move(float xOffset, float yOffset){ m_Point1.x += xOffset; m_Point1.y += yOffset; m_Point2.x += xOffset; m_Point2.y += yOffset; }
    void LineRenderer::SetPoint1(const Point2f& point){SetPoint1(point.x,point.y);}
    void LineRenderer::SetPoint1(float x, float y){ m_Point1.x = x; m_Point1.y = y; }
    void LineRenderer::SetPoint2(const Point2f& point){SetPoint2(point.x,point.y);}
    void LineRenderer::SetPoint2(float x, float y){ m_Point2.x = x; m_Point2.y = y; }
    Point2f LineRenderer::GetPoint1() const{return m_Point1;}
    Point2f LineRenderer::GetPoint2() const{return m_Point2;}
}
