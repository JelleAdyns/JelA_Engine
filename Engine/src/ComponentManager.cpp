#include "Utils.h"
#include "ComponentManager.h"
#include "GameObject.h"

namespace jela
{
    ComponentManager::AnyComponent::~AnyComponent()
    {
        if (m_pComponent)
        {
            std::destroy_at(m_pComponent);
            operator delete(m_pComponent, m_Alloc);
        }
    }
    ComponentManager::AnyComponent::AnyComponent(AnyComponent&& other) noexcept:
        m_pComponent{std::exchange(other.m_pComponent, nullptr)},
        m_Alloc{other.m_Alloc}
    {}
    ComponentManager::AnyComponent& ComponentManager::AnyComponent::operator=(AnyComponent&& other) noexcept
    {
        AnyComponent{std::move(other)}.swap(*this);
        return *this;
    }
    void ComponentManager::AnyComponent::swap(AnyComponent& other) noexcept
    {
        std::swap(m_pComponent, other.m_pComponent);
        std::swap(m_Alloc, other.m_Alloc);
    }
    void ComponentManager::RemoveComponent(const ComponentIndex& vecIndex)
    {
        if (!vecIndex.HasValue())
        {
            OutputDebugString(_T("Trying to remove a component be the index was std::nullopt. Continnuing..."));
            return;
        }

        const auto index = vecIndex.Get();
        utils::SwapEraseOnVector(m_Components, index);
        m_OnCompsChanged.NotifyObservers(
            CompsChangedInfo{ChangedIndex{.prevIndex = m_Components.size(), .newIndex = index}, index});
    }
    void ComponentManager::Clear()
    {
        // First destroy the components, then the allocators.
        const auto size = m_Components.size();
        m_Components.clear();
        for (std::size_t i = 0; i < size; ++i)
            m_OnCompsChanged.NotifyObservers(CompsChangedInfo{.removedComp = i});

        m_Allocs.clear();
    }


}
