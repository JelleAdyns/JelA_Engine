#ifndef SCENE_H
#define SCENE_H

#include <stdexcept>
#include <typeindex>
#include <unordered_map>

#include "Component.h"
#include "Engine.h"
#include "SingleLinkAllocators.h"
//#include "GameObject.h"

namespace jela
{
    class Scene final
    {

    private:
        class AnyComponent final
        {
        public:
            template <cDerivedComponent T, typename ...Args>
            static AnyComponent make_component(ComponentAllocator& alloc, Args... args);

            template <cDerivedComponent T> T* Get() const;

            ~AnyComponent();
            AnyComponent(const AnyComponent&) = delete;
            AnyComponent& operator=(const AnyComponent&) = delete;
            AnyComponent(AnyComponent&& other) noexcept; // Defined
            AnyComponent& operator=(AnyComponent&& other) noexcept; // Defined

            void swap(AnyComponent& other) noexcept;
            friend void swap(AnyComponent& a, AnyComponent& b) noexcept { a.swap(b); }

        private:
            template <cDerivedComponent T> AnyComponent(T* component, ComponentAllocator& alloc);

            Component* m_pComponent;
            std::reference_wrapper<ComponentAllocator> m_Alloc;
        };


        class ComponentHandler final
        {
        public:

            ComponentHandler() = default;
            ~ComponentHandler();
            ComponentHandler(const ComponentHandler& other) = default;
            ComponentHandler(ComponentHandler&& other) noexcept = default;
            ComponentHandler& operator=(const ComponentHandler& other) = default;
            ComponentHandler& operator=(ComponentHandler&& other) noexcept = default;

            template <cDerivedComponent T, typename ...Args> size_t AddComponent(Args ...args);
            void RemoveComponent(size_t vecIndex);
            template <cDerivedComponent T> T* GetComponent(size_t vecIndex);

        private:
            static constexpr std::size_t POOL_SIZE = 10'000;

            BufferAllocator<POOL_SIZE> m_ComponentAllocatorPool{};
            std::vector<AnyComponent> m_Components{};
            std::unordered_map<std::type_index, ComponentAllocator> m_Allocs{};
        };

    public:

        Scene() = default;

        template <cDerivedComponent T, typename ...Args>
        size_t AddComponent(Args ...args) { return m_ComponentHandler.AddComponent<T>(args...); }

        void RemoveComponent(size_t vecIndex) { m_ComponentHandler.RemoveComponent(vecIndex); }

        template <cDerivedComponent T>
        T* GetComponent(size_t vecIndex) { return m_ComponentHandler.GetComponent<T>(vecIndex); }

    private:

        ComponentHandler m_ComponentHandler{};
    };

    // ---------------------------------------------------------------------------------------------------------------
    // TEMPLATED DEFINITONS

    // AnyComponent
    template <cDerivedComponent T, typename ... Args>
    Scene::AnyComponent Scene::AnyComponent::make_component(ComponentAllocator& alloc, Args... args)
    {
        return AnyComponent{new (alloc) T{args...}, alloc};
    }
    template <cDerivedComponent T>
    T* Scene::AnyComponent::Get() const
    {
        if (auto p = dynamic_cast<T*>(m_pComponent); p != nullptr)
            return p;

        throw std::bad_typeid();
    }
    template <cDerivedComponent T>
    Scene::AnyComponent::AnyComponent(T* component, ComponentAllocator& alloc):
            m_pComponent{component},
            m_Alloc{alloc}
    {}

    // ComponentHandler
    template <cDerivedComponent T, typename ... Args>
    size_t Scene::ComponentHandler::AddComponent(Args... args)
    {
        const auto& typeID = typeid(T);
        if (!m_Allocs.contains(typeID))
        {
            auto [it, succeeded] = m_Allocs.try_emplace(typeID, std::type_identity<T>{}, m_ComponentAllocatorPool);
            if (!succeeded) throw std::runtime_error{std::format("Couldn't add typeID '{}'.", typeID.name())};
        }

        m_Components.emplace_back(AnyComponent::make_component<T>(m_Allocs.at(typeID), args...));
        return m_Components.size() - 1;
    }
    template <cDerivedComponent T>
    T* Scene::ComponentHandler::GetComponent(size_t vecIndex)
    {
        return m_Components.at(vecIndex).Get<T>();
    }
    // ---------------------------------------------------------------------------------------------------------------
}

#endif //SCENE_H
