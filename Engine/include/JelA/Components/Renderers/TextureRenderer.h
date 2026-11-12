#ifndef TEXTURERENDERCOMPONENT_H
#define TEXTURERENDERCOMPONENT_H
#include "JelA/Geometry.h"
#include "Renderers.h"

namespace jela
{
    class TextureImage;

    class TextureRenderer final : public Renderer
    {
    public:
        void Start() override;
        void Draw() const override;

        void SetOpacity(uint8_t opacityPercentage);
        void SetOpacity(float opacity);
        uint8_t GetOpacityPercentage() const;
        float GetOpacity() const;

    private:
        TextureImage* m_pTextureImage{};
        uint8_t m_OpacityPercentage{100};
        const Polygon poly{std::vector{
            Point2f{},
            Point2f{10,10},
            Point2f{20,40},
            Point2f{25,5},
            Point2f{20,-5},
            Point2f{15,-10}
            }};
        const Arc arc{60, 0, 20,40,-30,200,true};
        const Arc arc2{{-10,80}, {10, 100}, false, true};
    };

} // jela

#endif //TEXTURERENDERCOMPONENT_H
