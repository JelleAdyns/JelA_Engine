#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include "Component.h"
#include "Engine.h"
#include "TransformComponent.h"

namespace jela
{
    class GameObject final
    {
    public:

        explicit GameObject();

        ~GameObject();
        // disable copying because it would mean creating new components
        GameObject(const GameObject& other) = delete;
        GameObject& operator=(const GameObject& other) = delete;

        GameObject(GameObject&& other) noexcept; // Defined
        GameObject& operator=(GameObject&& other) noexcept = delete;

        template <cDerivedComponent T, typename ...Args>
        void AddComponent(Args&&... args)
        {
            if (const auto& typeID = typeid(T);
                !HasComponent(typeID))
            {
                m_Components[typeID] = ENGINE.ComponentMngr()->AddComponent<T>(args...);
            }
            else throw std::runtime_error("Object already owns a reference to an instance of the passed component type." );
        }

        template <cDerivedComponent T>
        void RemoveComponent()
        {
            if (const auto& typeID = typeid(T);
                HasComponent(typeID))
            {
                const auto index = m_Components.at(typeID);
                ENGINE.ComponentMngr()->RemoveComponent(index);
                m_Components.erase(typeID);
            }
        }

        template <cDerivedComponent T>
        T* GetComponent() const
        {
            if constexpr (std::is_same_v<T, TransformComponent>)
            {
                if (m_Transform) return m_Transform;
            }

            if (const auto& typeID = typeid(T);
                HasComponent(typeID))
            {
                return ENGINE.ComponentMngr()->GetComponent<T>(m_Components.at(typeID));
            }

            throw std::runtime_error("Object doesn't own a reference to an instance of the passed component type.");
        }

        template <cDerivedComponent T>
        bool HasComponent() const { return HasComponent(typeid(T)); }
        bool HasComponent(const std::type_index& typeID) const { return m_Components.contains(typeID); }

    private:

        TransformComponent* m_Transform{nullptr};
        std::unordered_map<std::type_index, size_t> m_Components{};
    };
}

#endif //GAMEOBJECT_H
