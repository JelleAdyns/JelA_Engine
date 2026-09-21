#ifndef TEXTURECOMPONENT_H
#define TEXTURECOMPONENT_H
#include "Component.h"
#include "ResourceManager.h"
#include "Structs.h"

namespace jela
{
    class RectTransform;

    class TextureImage final : public Component
    {
    public:

        TextureImage(const tstring& textureName);

        void Start() override;
        ResourcePtr<Texture> GetTexture() const;
        const Texture* GetRawTexture() const;

        Rectf GetDestRect() const;

    private:
        ResourcePtr<Texture> m_pTexture{};
        RectTransform* m_pRectTransform{};
    };

} // jela

#endif //TEXTURECOMPONENT_H
