#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H
#include "Component.h"
#include "Structs.h"
#include "Scene.h"

namespace jela
{
    class TransformComponent final : public Component
    {
    public:
        static constexpr std::size_t MAX_AMOUNT = Scene::GetMaxObjects();

    private:
        Vector2f m_Translation{};
        float m_Rotation{};
        Vector2f m_Scale{1.f, 1.f};
    };
} // jela

#endif //TRANSFORMCOMPONENT_H
