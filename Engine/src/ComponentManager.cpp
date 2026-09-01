#include "Utils.h"
#include "ComponentManager.h"
#include "GameObject.h"

namespace jela
{
    Component* ComponentManager::AnyComponent::GetBasePointer() const
    {
        return m_pComponent;
    }
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
    void ComponentManager::RemoveComponent(const Component* pCompToRemove)
    {
        const auto index = pCompToRemove->GetBufferIndex();
        utils::SwapEraseOnVector(m_Components, index);
        if (m_Components.empty()) return;
        m_Components.at(index).GetBasePointer()->SetBufferIndex(BufferOwnerKey{}, index);
    }
    void ComponentManager::Clear()
    {
        // First destroy the components, then the allocators.
        m_Components.clear();
        m_Allocs.clear();
    }


}
