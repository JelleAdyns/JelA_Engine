#ifndef SCENE_H
#define SCENE_H

#include <stdexcept>
#include <typeindex>
#include <unordered_map>

#include "Component.h"
#include "ComponentAllocator.h"
#include "Engine.h"
#include "FixedSizeAllocator.h"

namespace jela
{
    class Scene final
    {

    private:
        class AnyComponent
        {
        public:
            template <cDerivedComponent T, typename ...Args>
            static AnyComponent make_component(ComponentAllocator& alloc, Args... args)
            {
                return AnyComponent{new (operator new(sizeof(T), alloc)) T{args...}, alloc};
            }

            template <cDerivedComponent T>
            T* Get() const
            {
                if (auto p = dynamic_cast<T*>(m_pComponent); p != nullptr)
                    return p;

                throw std::bad_typeid();
            }

            ~AnyComponent();

            AnyComponent(const AnyComponent&) = delete;
            AnyComponent& operator=(const AnyComponent&) = delete;

            AnyComponent(AnyComponent&& other) noexcept; // Defined
            AnyComponent& operator=(AnyComponent&& other) noexcept; // Defined

            void swap(AnyComponent& other) noexcept;
            friend void swap(AnyComponent& a, AnyComponent& b) noexcept { a.swap(b); }

        private:

            explicit AnyComponent(cDerivedComponent auto* component, ComponentAllocator& alloc):
                m_pComponent{component},
                m_Alloc{alloc}
            {}

            Component* m_pComponent;
            ComponentAllocator& m_Alloc;
        };


    public:

        Scene() = default;

        template <cDerivedComponent T, typename ...Args>
        size_t AddComponent(Args ...args)
        {
            if (!m_Allocs.contains(typeid(T)))
            {
                //m_Components.emplace_back(sizeof(T), alignof(T), T::GetMaxAmount());

                auto [it, succeeded] = m_Allocs.try_emplace(typeid(T), sizeof(T), alignof(T), T::GetMaxAmount());
                if (!succeeded) throw std::runtime_error{std::format("Couldn't add typeID '{}'.", typeid(T).name())};
                //m_Allocs[typeid(T)] = m_Components.size() - 1;
            }

            m_Components.emplace_back(AnyComponent::make_component<T>(m_Allocs.at(typeid(T)), args...));
            return m_Components.size() - 1;
        }

        template <cDerivedComponent T>
        void RemoveComponent(size_t vecIndex)
        {
            //auto& vec = GetComponentVector<T>();
            jela::utils::SwapEraseOnVector(m_Components, vecIndex);

            //const auto index = m_IndicesToComponenTypes[typeid(T)];

            //if (vec.empty()) jela::utils::SwapEraseOnVector(m_Components, index);
        }

        template <cDerivedComponent T>
        T* GetComponent(size_t vecIndex)
        {
            const AnyComponent& anyCompObject = m_Components.at(vecIndex);
            return anyCompObject.Get<T>();
        }

    private:

        // template <cDerivedComponent T>
        // std::vector<AnyComponent>& GetComponentVector()
        // {
        //     if (!m_IndicesToComponenTypes.contains(typeid(T)))
        //         throw std::bad_typeid{};
        //
        //     const auto index = m_IndicesToComponenTypes[typeid(T)];
        //
        //     if (index >= m_Components.size())
        //         throw std::out_of_range{"An index in the map was found while there was no corresponding vector. This should never happen!"};
        //
        //     return m_Components.at(index);
        // }

        std::vector<AnyComponent> m_Components{};
        std::unordered_map<std::type_index, ComponentAllocator> m_Allocs{};

        //std::vector<std::vector<AnyComponent>> m_Components{};
        //std::unordered_map<std::type_index, size_t> m_IndicesToComponenTypes{};

    };


}

#endif //SCENE_H
