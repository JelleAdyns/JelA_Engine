#include "../include/Scene.h"

namespace jela
{
    Scene::AnyComponent::~AnyComponent()
    {
        if (m_pComponent)
        {
            m_pComponent->~Component();
            operator delete(m_pComponent, m_Alloc);
        }
    }
    Scene::AnyComponent::AnyComponent(AnyComponent&& other) noexcept:
        m_pComponent{std::exchange(other.m_pComponent, nullptr)},
        m_Alloc{other.m_Alloc}
    {}
    Scene::AnyComponent& Scene::AnyComponent::operator=(AnyComponent&& other) noexcept
    {
        AnyComponent{std::move(other)}.swap(*this);
        return *this;
    }
    void Scene::AnyComponent::swap(AnyComponent& other) noexcept
    {
        std::swap(m_pComponent, other.m_pComponent);
        std::swap(m_Alloc, other.m_Alloc);
    }
}
