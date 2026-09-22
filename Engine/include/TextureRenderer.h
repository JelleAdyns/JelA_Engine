#ifndef TEXTURERENDERCOMPONENT_H
#define TEXTURERENDERCOMPONENT_H
#include "Renderer.h"

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
    };

} // jela

#endif //TEXTURERENDERCOMPONENT_H
