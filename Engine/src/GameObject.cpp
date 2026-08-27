#include "GameObject.h"

namespace jela
{
    GameObject::GameObject()
    {
        AddComponent<TransformComponent>();
        m_Transform = GetComponent<TransformComponent>();
    }
    GameObject::~GameObject()
    {
        const auto mngr = ENGINE.ComponentMngr();

        for (auto& index : m_Components | std::views::values)
            index.UnbindObserver();
        for (const auto& index : m_Components | std::views::values)
            mngr->RemoveComponent(index);

        m_Components.clear();
    }
    GameObject::GameObject(GameObject&& other) noexcept:
        m_Transform{std::exchange(other.m_Transform, nullptr)},
        m_Components{std::exchange(other.m_Components, {})}
    {}
}
