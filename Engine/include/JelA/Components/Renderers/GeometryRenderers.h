#ifndef GEOMETRYRENDERERS_H
#define GEOMETRYRENDERERS_H
#include "ShapeRenderers.h"
#include "JelA/Components/Arc.h"
#include "JelA/Components/Polygon.h"
#include "JelA/GameObject.h"

namespace jela
{
    template <bool NEVER_USE_OUTLINE>
    class PolygonRendererBase : public FillableRenderer<NEVER_USE_OUTLINE>
    {
    public:
        void Init() override
        {
            m_pPolygon = Component::GetOwner()->GetComponent<Polygon>();
        }
        void BeforeDraw() override
        {
            m_pPolygon->UpdateGeometry();
        }

    protected:
        void Fill(RenderManager* r) const override
        {
            r->FillPolygon(m_pPolygon->GetPolygon());
        }
        void Line(RenderManager* r, float lineThickness, const StrokeStyle* ss) const override
        {
            r->DrawPolygon(m_pPolygon->GetPolygon(), lineThickness, ss);
        }
        private:
        Polygon* m_pPolygon{};
    };
    using PolygonRenderer = PolygonRendererBase<false>;
    using PolygonRenderer_DiscardOutline = PolygonRendererBase<true>;


    template <bool NEVER_USE_OUTLINE>
    class ArcRendererBase : public FillableRenderer<NEVER_USE_OUTLINE>
    {
    public:
        void Init() override
        {
            m_pArc = Component::GetOwner()->GetComponent<Arc>();
        }
        void BeforeDraw() override
        {
            m_pArc->UpdateGeometry();
        }

    protected:
        void Fill(RenderManager* r) const override
        {
            r->FillArc(m_pArc->GetArc());
        }
        void Line(RenderManager* r, float lineThickness, const StrokeStyle* ss) const override
        {
            r->DrawArc(m_pArc->GetArc(), lineThickness, ss);
        }
    private:
        Arc* m_pArc{};
    };

    using ArcRenderer = ArcRendererBase<false>;
    using ArcRenderer_DiscardOutline = ArcRendererBase<true>;

}


#endif //GEOMETRYRENDERERS_H
