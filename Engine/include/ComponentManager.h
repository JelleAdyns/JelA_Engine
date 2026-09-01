#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <stdexcept>
#include <typeindex>
#include <unordered_map>

#include "Component.h"
#include "Observer.h"
#include "ObservingObjects.h"
#include "SingleLinkAllocators.h"

namespace jela
{
    struct CompsChangedInfo;
    class ComponentManager final
    {
    private:
        class AnyComponent final
        {
        public:

            template <cDerivedComponent T, typename ...Args>
            static AnyComponent make_component(ComponentAllocator& alloc, Args... args);

            template <cDerivedComponent T> T* Get() const;
            Component* GetBasePointer() const;

            ~AnyComponent();

            AnyComponent(const AnyComponent&) = delete;
            AnyComponent& operator=(const AnyComponent&) = delete;
            AnyComponent(AnyComponent&& other) noexcept;
            AnyComponent& operator=(AnyComponent&& other) noexcept;

            void swap(AnyComponent& other) noexcept;
            friend void swap(AnyComponent& a, AnyComponent& b) noexcept { a.swap(b); }

        private:
            template <cDerivedComponent T> AnyComponent(T* component, ComponentAllocator& alloc);

            Component* m_pComponent;
            std::reference_wrapper<ComponentAllocator> m_Alloc;
        };
    public:
        struct ChangedIndex
        {
            std::size_t prevIndex{};
            std::size_t newIndex{};
        };

        ComponentManager() = default;
        ~ComponentManager() { Clear(); }
        ComponentManager(const ComponentManager& other) = delete;
        ComponentManager(ComponentManager&& other) noexcept = delete;
        ComponentManager& operator=(const ComponentManager& other) = delete;
        ComponentManager& operator=(ComponentManager&& other) noexcept = delete;

        template <cDerivedComponent T, typename ...Args> T* AddComponent(Args ...args);
        void RemoveComponent(const Component* pCompToRemove);
        template <template<typename> typename Container>
        void RemoveComponents(const Container<Component*>& pCompsToRemove);
        template <cDerivedComponent T> T* GetComponent(std::size_t vecIndex);

        void Clear();

    private:
        static constexpr std::size_t POOL_SIZE = 10'000;

        BufferAllocator<POOL_SIZE> m_ComponentAllocatorPool{};
        std::vector<AnyComponent> m_Components{};
        std::unordered_map<std::type_index, ComponentAllocator> m_Allocs{};
        //Subject<const CompsChangedInfo&> m_OnCompsChanged{};
    };

    struct CompsChangedInfo
    {
        std::optional<ComponentManager::ChangedIndex> changedIndexInfo{};
        std::size_t removedComp{};
    };
// ---------------------------------------------------------------------------------------------------------------
// TEMPLATED DEFINITONS

    // AnyComponent
    template <cDerivedComponent T, typename ... Args>
    ComponentManager::AnyComponent ComponentManager::AnyComponent::make_component(ComponentAllocator& alloc, Args... args)
    {
        return AnyComponent{new (alloc) T{args...}, alloc};
    }
    template <cDerivedComponent T>
    T* ComponentManager::AnyComponent::Get() const
    {
        if (auto p = dynamic_cast<T*>(m_pComponent); p != nullptr)
            return p;

        throw std::bad_typeid();
    }
    template <cDerivedComponent T>
    ComponentManager::AnyComponent::AnyComponent(T* component, ComponentAllocator& alloc):
            m_pComponent{component},
            m_Alloc{alloc}
    {}

    // ComponentHandler
    template <cDerivedComponent T, typename ... Args>
    T* ComponentManager::AddComponent(Args... args)
    {
        const auto& typeID = typeid(T);
        if (!m_Allocs.contains(typeID))
        {
            auto [it, succeeded] = m_Allocs.try_emplace(typeID, std::type_identity<T>{}, m_ComponentAllocatorPool);
            if (!succeeded) throw std::runtime_error{std::format("Couldn't add typeID '{}'.", typeID.name())};
        }

        m_Components.emplace_back(AnyComponent::make_component<T>(m_Allocs.at(typeID), args...));
        auto typeComp = m_Components.back().Get<T>();
        typeComp->SetBufferIndex(BufferOwnerKey{}, m_Components.size() - 1);
        return typeComp;
    }
    template <template <typename> class Container>
    void ComponentManager::RemoveComponents(const Container<Component*>& pCompsToRemove)
    {
        if (pCompsToRemove.empty()) return;

        std::size_t firstRemovedIndex{std::numeric_limits<std::size_t>::max()};
        std::erase_if(m_Components, [&pCompsToRemove, &firstRemovedIndex](const AnyComponent& anyComp)
        {
            const bool remove = std::ranges::any_of(pCompsToRemove,
                [&anyComp](const Component* pCompToRemove)
                {
                    return anyComp.GetBasePointer() == pCompToRemove;
                });

            if (remove)
            {
                if (const auto curIndex = anyComp.GetBasePointer()->GetBufferIndex(); curIndex < firstRemovedIndex)
                    firstRemovedIndex = curIndex;
            }

            return remove;
        });

        std::ranges::for_each(m_Components | std::views::drop(firstRemovedIndex),
            [i = firstRemovedIndex](const AnyComponent& anyComp) mutable
            {
                anyComp.GetBasePointer()->SetBufferIndex(BufferOwnerKey{}, i);
                ++i;
            });
    }
    template <cDerivedComponent T>
    T* ComponentManager::GetComponent(std::size_t vecIndex)
    {
        return m_Components.at(vecIndex).Get<T>();
    }
// ---------------------------------------------------------------------------------------------------------------
}
#endif //COMPONENTMANAGER_H
