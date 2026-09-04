#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <complex>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include "Component.h"
#include "Scene.h"
#include "Engine.h"

namespace jela
{
    class RenderComponent;
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
                T* pComp = m_pScene->AddComponent<T>(ComponentOwnerKey{}, args...);
                pComp->SetOwner(ComponentOwnerKey{}, this);
                m_Components[typeID] = pComp;

                if constexpr (std::is_base_of_v<RenderComponent, T>)
                    m_pRenderComp = pComp;

                pComp->Init();

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
                const auto pComp = m_Components.at(typeID);
                m_pScene->RemoveComponent(ComponentOwnerKey{}, pComp);
                m_Components.erase(typeID);
            }
        }

        template <cDerivedComponent T>
        T* GetComponent() const
        {
            if constexpr (std::is_same_v<T, TransformComponent>)
            {
                if (m_pTransform) return m_pTransform;
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

        void Start();
        void Draw() const;
        void Update();

        void MarkDead();
        bool IsDead() const { return m_IsDead; }

        const std::vector<GameObject*>& Children() const { return m_pChildren; }
        TransformComponent* Transform() const { return m_pTransform; }
        GameObject* Parent() const { return m_pParent; }

        void SetParent(GameObject& pParent, bool keepWorldPosition);
        void SetParent(GameObject* pParent, bool keepWorldPosition);
        bool IsChild(const GameObject& pGameObject) const;
        bool IsChild(const GameObject* pGameObject) const;

        bool IsPartOfScene() const { return m_IsPartOfScene; }
        const Scene* OwnerScene() const { return m_pScene; }
    private:
        explicit GameObject(Scene& scene);

        bool m_IsPartOfScene{false};
        bool m_IsDead{false};
        Scene* m_pScene;

        // Tree
        GameObject* m_pParent{};
        std::vector<GameObject*> m_pChildren{};

        // Components
        std::unordered_map<std::type_index, Component*> m_Components{};
        TransformComponent* m_pTransform;
        RenderComponent* m_pRenderComp{nullptr};

    };
}

#endif //GAMEOBJECT_H
