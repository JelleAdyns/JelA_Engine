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
        Rectf GetSourceRect() const;
        void SetSourceRectPos(float x, float y);
        void SetSourceRectPos(Vector2f newPos);
        void SetSourceRectSize(float width, float height);
        void SetSourceRectSize(Vector2f size);
        void SetSourceRect(Rectf);

    private:
        ResourcePtr<Texture> m_pTexture{};
        RectTransform* m_pRectTransform{};
        Rectf m_SourceRect{};
    };

} // jela

#endif //TEXTURECOMPONENT_H
