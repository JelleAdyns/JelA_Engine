#ifndef TEXTURECOMPONENT_H
#define TEXTURECOMPONENT_H
#include "Component.h"
#include "ResourceManager.h"
#include "Structs.h"

namespace jela
{
    class AnchoredRect;

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
        AnchoredRect* m_pAnchoredRect{};
    };

} // jela

#endif //TEXTURECOMPONENT_H
