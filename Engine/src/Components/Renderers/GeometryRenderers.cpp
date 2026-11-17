#include "JelA/Components/Renderers/GeometryRenderers.h"

#include "JelA/Engine.h"
#include "JelA/GameObject.h"

namespace jela
{
    void PolygonRenderer::Init()
    {
        m_pPolygon = GetOwner()->GetComponent<Polygon>();
    }
    void PolygonRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();

        r->SetColor(GetColor());
        if (m_Fill) r->FillPolygon(m_pPolygon->GetPolygon());
        else r->DrawPolygon(m_pPolygon->GetPolygon(), GetLineThickness());
    }
    void PolygonRenderer::SetFill(bool fill)
    {
        m_Fill = fill;
    }
    void ArcRenderer::Init()
    {
        m_pArc = GetOwner()->GetComponent<Arc>();
    }
    void ArcRenderer::Draw() const
    {
        const auto r = ENGINE.RenderMngr();

        r->SetColor(GetColor());
        if (m_Fill) r->FillArc(m_pArc->GetArc());
        else r->DrawArc(m_pArc->GetArc(), GetLineThickness());
    }
    void ArcRenderer::SetFill(bool fill)
    {
        m_Fill = fill;
    }
}
