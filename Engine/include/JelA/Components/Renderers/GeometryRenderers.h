#ifndef GEOMETRYRENDERERS_H
#define GEOMETRYRENDERERS_H
#include "ShapeRenderers.h"
#include "JelA/Components/Polygon.h"

namespace jela
{
    class PolygonRenderer final : public ShapeRenderer
    {
    public:
        void Init() override;
        void Draw() const override;

        void SetFill(bool fill);

    private:
        bool m_Fill{false};
        Polygon* m_pPolygon{};
    };
}


#endif //GEOMETRYRENDERERS_H
