#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <complex>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include "Component.h"
#include "Engine.h"

namespace jela
{
    class Scene;
    class TransformComponent;
    class GameObject final
    {
    public:

        static GameObject& Create(Scene& scene);

        ~GameObject();
        // disable copying because it would mean creating new components
        GameObject(const GameObject& other) = delete;
        GameObject& operator=(const GameObject& other) = delete;

        GameObject(GameObject&& other) noexcept; // Defined
        GameObject& operator=(GameObject&& other) noexcept = delete;

        template <cDerivedComponent T, typename ...Args>
        T* AddComponent(Args&&... args)
        {
            if (const auto& typeID = typeid(T);
                !HasComponent(typeID))
            {
                T* pComp = ENGINE.ComponentMngr()->AddComponent<T>(args...);
                pComp->SetOwner(ComponentOwnerKey{}, this);
                m_Components[typeID] = pComp;

                return pComp;
            }

            throw std::runtime_error("Object already owns a reference to an instance of the passed component type." );
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
                if (auto p = dynamic_cast<T*>(m_Components.at(typeID)); p != nullptr)
                    return p;

                throw std::bad_typeid();
            }

            throw std::runtime_error("Object doesn't own a reference to an instance of the passed component type.");
        }

        template <cDerivedComponent T>
        bool HasComponent() const { return HasComponent(typeid(T)); }
        bool HasComponent(const std::type_index& typeID) const { return m_Components.contains(typeID); }

    private:
        explicit GameObject();

        TransformComponent* m_Transform{nullptr};
        std::unordered_map<std::type_index, Component*> m_Components{};
    };
}

#endif //GAMEOBJECT_H
