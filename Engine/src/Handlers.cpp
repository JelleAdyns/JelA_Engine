#include "ComponentHandler.h"
#include "GameObjectHandler.h"
#include "Scene.h"
#include "GameObject.h"

namespace jela
{
    ComponentHandler::AnyComponent::~AnyComponent()
    {
        if (m_pComponent)
        {
            m_pComponent->~Component();
            operator delete(m_pComponent, m_Alloc);
        }
    }
    ComponentHandler::AnyComponent::AnyComponent(AnyComponent&& other) noexcept:
        m_pComponent{std::exchange(other.m_pComponent, nullptr)},
        m_Alloc{other.m_Alloc}
    {}
    ComponentHandler::AnyComponent& ComponentHandler::AnyComponent::operator=(AnyComponent&& other) noexcept
    {
        AnyComponent{std::move(other)}.swap(*this);
        return *this;
    }
    void ComponentHandler::AnyComponent::swap(AnyComponent& other) noexcept
    {
        std::swap(m_pComponent, other.m_pComponent);
        std::swap(m_Alloc, other.m_Alloc);
    }
    void ComponentHandler::RemoveComponent(size_t vecIndex)
    {
        utils::SwapEraseOnVector(m_Components, vecIndex);
    }
    void ComponentHandler::Clear()
    {
        // First destroy the components, then the allocators.
        m_Components.clear();
        m_Allocs.clear();
    }

    GameObjectHandler::GameObjectHandler():
        m_GameObjectAlloc{std::type_identity<GameObject>{}, MAX_GAME_OBJECTS}
    {
    }
    GameObject& GameObjectHandler::AddGameObject(Scene& scene)
    {
        m_pGameObjects.emplace_back(new (m_GameObjectAlloc) GameObject{scene});
        return *m_pGameObjects.back();
    }
    GameObject& GameObjectHandler::ConsumeGameObject(GameObject&& gameObject)
    {
        auto* p = static_cast<GameObject*>(m_GameObjectAlloc.Acquire(sizeof(GameObject)));
        p = new (p) GameObject{std::move(gameObject)};
        return *(m_pGameObjects.emplace_back(p));
    }
    void GameObjectHandler::Clear()
    {
        for (const auto pObject : m_pGameObjects)
        {
            pObject->~GameObject();
            operator delete(pObject, m_GameObjectAlloc);
        }
        m_pGameObjects.clear();
    }
}
