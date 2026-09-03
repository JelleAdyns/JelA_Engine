#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h"

namespace jela
{
    void Scene::Draw() const
    {
        for (const auto & obj : m_GameObjectHandler.GameObjects())
        {
            auto pos = obj->Transform()->WorldPosition();
            ENGINE.DrawEllipse(Point2f{pos.x, pos.y}, 10.f, 20.f);
        }
    }
    GameObject& Scene::AddGameObject()
    {
        return GameObject::Create(*this);
    }
    GameObject& Scene::ConsumeGameObject(GameObject&& gameObject)
    {
        return std::forward<GameObject&>(m_GameObjectHandler.ConsumeGameObject(std::move(gameObject)));
    }
    void Scene::RemoveComponent(ComponentOwnerKey, const Component* pCompToRemove)
    {
        m_ComponentHandler.RemoveComponent(pCompToRemove);
    }

    Scene::GameObjectHandler::GameObjectHandler(SingleLinkAllocator& alloc):
        m_GameObjectAlloc{std::type_identity<GameObject>{}, MAX_GAME_OBJECTS, alloc}
    {}
    GameObject& Scene::GameObjectHandler::ConsumeGameObject(GameObject&& gameObject)
    {
        auto* p = static_cast<GameObject*>(m_GameObjectAlloc.Acquire(sizeof(GameObject)));
        p = new (p) GameObject{std::move(gameObject)};
        return *(m_pGameObjects.emplace_back(p));
    }
    void Scene::GameObjectHandler::Clear()
    {
        for (const auto pObject : m_pGameObjects)
            DestroyGameObject(pObject);

        m_pGameObjects.clear();
    }
    void Scene::GameObjectHandler::DestroyGameObject(GameObject* pGameObject)
    {
        if (!pGameObject) return;
        std::destroy_at(pGameObject);
        operator delete(pGameObject, m_GameObjectAlloc);
    }

    Component* Scene::ComponentHandler::AnyComponent::GetBasePointer() const
    {
        return m_pComponent;
    }
    Scene::ComponentHandler::AnyComponent::~AnyComponent()
    {
        if (m_pComponent)
        {
            std::destroy_at(m_pComponent);
            operator delete(m_pComponent, m_Alloc);
        }
    }
    Scene::ComponentHandler::AnyComponent::AnyComponent(AnyComponent&& other) noexcept:
        m_pComponent{std::exchange(other.m_pComponent, nullptr)},
        m_Alloc{other.m_Alloc}
    {}
    Scene::ComponentHandler::AnyComponent& Scene::ComponentHandler::AnyComponent::operator=(AnyComponent&& other) noexcept
    {
        AnyComponent{std::move(other)}.swap(*this);
        return *this;
    }
    void Scene::ComponentHandler::AnyComponent::swap(AnyComponent& other) noexcept
    {
        std::swap(m_pComponent, other.m_pComponent);
        std::swap(m_Alloc, other.m_Alloc);
    }
    Scene::ComponentHandler::ComponentHandler(SingleLinkAllocator& alloc):
        m_rAlloc{alloc}
    {}
    void Scene::ComponentHandler::RemoveComponent(const Component* pCompToRemove)
    {
        const auto index = pCompToRemove->GetBufferIndex();
        utils::SwapEraseOnVector(m_Components, index);
        if (m_Components.empty()) return;
        m_Components.at(index).GetBasePointer()->SetBufferIndex(BufferOwnerKey{}, index);
    }
    void Scene::ComponentHandler::Clear()
    {
        // First destroy the components, then the allocators.
        m_Components.clear();
        m_Allocs.clear();
    }
}
