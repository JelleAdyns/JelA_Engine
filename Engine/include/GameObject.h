#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include "Component.h"
#include "Scene.h"

namespace jela
{
    class GameObject final
    {
    public:
        GameObject(Scene&scene):
            m_pScene{scene}
        {
            AddComponent<derived>();
            AddComponent<derived2>();
            const auto comp = GetComponent<derived>();
            comp->kaas = 4;
        };

        template <cDerivedComponent T, typename ...Args>
        void AddComponent(Args&&... args)
        {
            if (const auto& typeID = typeid(T);
                !HasComponent(typeID))
            {
                m_Components[typeID] = m_pScene.AddComponent<T>(args...);
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
                m_pScene.RemoveComponent(index);
                m_Components.erase(typeID);
            }
        }

        template <cDerivedComponent T>
        T* GetComponent() const
        {
            if (const auto& typeID = typeid(T);
                HasComponent(typeID))
            {
                return m_pScene.GetComponent<T>(m_Components.at(typeID));
            }
            throw std::runtime_error("Object doesn't own a reference to an instance of the passed component type.");
        }

        template <cDerivedComponent T>
        bool HasComponent() const { return HasComponent(typeid(T)); }
        bool HasComponent(const std::type_index& typeID) const { return m_Components.contains(typeID); }

    private:

        Scene& m_pScene;
        std::unordered_map<std::type_index, size_t> m_Components{};
    };
}

#endif //GAMEOBJECT_H
