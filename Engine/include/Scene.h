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
            static AnyComponent make_component(Args... args)
            {
                const auto& typeID = typeid(T);
                OutputDebugStringA(std::format("Name T: {}\n",typeID.name()).c_str());
                if (!m_Allocs.contains(typeID))
                {
                    auto [it, succeeded] = m_Allocs.try_emplace(typeID, sizeof(T), alignof(T), T::GetMaxAmount());
                    if (!succeeded) throw std::runtime_error{std::format("Couldn't add typeID '{}'.", typeID.name())};
                }

                return AnyComponent{new (operator new(sizeof(T), m_Allocs.at(typeID))) T{args...}};
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

            template <cDerivedComponent T>
            explicit AnyComponent(T* component):
                m_pComponent{component},
                m_TypeID{typeid(T)}
            {}

            Component* m_pComponent;
            std::type_index m_TypeID;
            static inline std::unordered_map<std::type_index, ComponentAllocator> m_Allocs{};
        };


    public:

        Scene() = default;

        template <cDerivedComponent T, typename ...Args>
        size_t AddComponent(Args ...args)
        {
            m_Components.emplace_back(AnyComponent::make_component<T>(args...));
            return m_Components.size() - 1;
        }

        void RemoveComponent(size_t vecIndex)
        {
            jela::utils::SwapEraseOnVector(m_Components, vecIndex);
        }

        template <cDerivedComponent T>
        T* GetComponent(size_t vecIndex)
        {
            return m_Components.at(vecIndex).Get<T>();
        }

    private:

        std::vector<AnyComponent> m_Components{};
    };


}

#endif //SCENE_H
