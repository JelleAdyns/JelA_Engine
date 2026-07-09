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
            const auto comp = GetComponent<derived>();
            comp->kaas = 4;
        };

        template <cDerivedComponent T, typename ...Args>
        void AddComponent(Args&&... args)
        {
            if (!HasComponent<T>()) m_Components[typeid(T)] = m_pScene.AddComponent<T>(args...);
            else throw std::runtime_error("Object already owns a reference to an instance of the passed component type." );
        }

        template <cDerivedComponent T>
        void RemoveComponent()
        {
            if (HasComponent<T>())
            {
                const auto index = m_Components.at(typeid(T));
                m_pScene.RemoveComponent<T>(index);
                m_Components.erase(typeid(T));
            }
        }

        template <cDerivedComponent T>
        T* GetComponent() const
        {
            if (HasComponent<T>()) return m_pScene.GetComponent<T>(m_Components.at(typeid(T)));
            throw std::runtime_error("Object doesn't own a reference to an instance of the passed component type.");
        }

        template <cDerivedComponent T>
        bool HasComponent() const { return m_Components.contains(typeid(T)); }

    private:

        Scene& m_pScene;
        std::unordered_map<std::type_index, size_t> m_Components{};
    };
}

#endif //GAMEOBJECT_H
