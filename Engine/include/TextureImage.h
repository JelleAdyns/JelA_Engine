#ifndef TEXTURECOMPONENT_H
#define TEXTURECOMPONENT_H
#include "Component.h"
#include "ResourceManager.h"
#include "Structs.h"

namespace jela
{
    class RectTransform;

    class TextureImage : public Component
    {
    public:

        TextureImage(const tstring& textureName);

        void Start() override;
        const ResourcePtr<Texture>& GetManagedTexture() const;
        const Texture* GetTexture() const;

        Rectf GetDestRect() const;
        Rectf GetSourceRect() const;
        void SetSourceRectPos(float x, float y);
        void SetSourceRectPos(Vector2f newPos);
        void SetSourceRectSize(float width, float height);
        void SetSourceRectSize(Vector2f size);
        void SetSourceRect(Rectf);
        void UseSourceRectSize();
        void UseTextureSize();

    private:
        ResourcePtr<Texture> m_pTexture{};
        RectTransform* m_pRectTransform{};
        Rectf m_SourceRect{};
        Vector2f m_DestSize{};
    };

} // jela

#endif //TEXTURECOMPONENT_H
