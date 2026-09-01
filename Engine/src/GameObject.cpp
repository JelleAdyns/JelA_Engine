#include "GameObject.h"

#include "Scene.h"

namespace jela
{
    GameObject& GameObject::Create(Scene& scene)
    {
        return scene.ConsumeGameObject(std::move(GameObject{}));
    }
    GameObject::GameObject()
    {
        m_Transform = AddComponent<TransformComponent>();
    }
    GameObject::~GameObject()
    {
        const auto mngr = ENGINE.ComponentMngr();

        mngr->RemoveComponents<std::vector>(m_Components | std::views::values | std::ranges::to<std::vector>());
        m_Components.clear();
    }
    GameObject::GameObject(GameObject&& other) noexcept:
        m_Transform{std::exchange(other.m_Transform, nullptr)},
        m_Components{std::exchange(other.m_Components, {})}
    {}
}
