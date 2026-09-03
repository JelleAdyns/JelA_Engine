#ifndef SCENE_H
#define SCENE_H

#include "GameObject.h"

namespace jela
{
    class Scene final
    {
    private:
        class GameObjectHandler final
        {
        public:
            explicit GameObjectHandler(SingleLinkAllocator& alloc);
            ~GameObjectHandler() { Clear(); }
            GameObjectHandler(const GameObjectHandler& other) = delete;
            GameObjectHandler(GameObjectHandler&& other) noexcept = default;
            GameObjectHandler& operator=(const GameObjectHandler& other) = delete;
            GameObjectHandler& operator=(GameObjectHandler&& other) noexcept = delete;

            GameObject& ConsumeGameObject(GameObject&& gameObject);
            void Clear();

            static constexpr std::size_t MAX_GAME_OBJECTS = 100;
            const std::vector<GameObject*>& GameObjects() const {return m_pGameObjects;}
        private:

            void DestroyGameObject(GameObject* pGameObject);

            FixedSizeAllocator m_GameObjectAlloc;
            std::vector<GameObject*> m_pGameObjects{};
        };

        class ComponentHandler final
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
            explicit ComponentHandler(SingleLinkAllocator& alloc);
            ~ComponentHandler() { Clear(); }
            ComponentHandler(const ComponentHandler& other) = delete;
            ComponentHandler(ComponentHandler&& other) noexcept = delete;
            ComponentHandler& operator=(const ComponentHandler& other) = delete;
            ComponentHandler& operator=(ComponentHandler&& other) noexcept = delete;

            template <cDerivedComponent T, typename ...Args> T* AddComponent(Args ...args);
            void RemoveComponent(const Component* pCompToRemove);
            template <template<typename> typename Container>
            void RemoveComponents(const Container<Component*>& pCompsToRemove);
            template <cDerivedComponent T> T* GetComponent(std::size_t vecIndex);

            void Clear();

        private:
            SingleLinkAllocator& m_rAlloc;
            std::vector<AnyComponent> m_Components{};
            std::unordered_map<std::type_index, ComponentAllocator> m_Allocs{};
        };

    public:

        static constexpr std::size_t GetMaxObjects() { return GameObjectHandler::MAX_GAME_OBJECTS; };

        void Draw() const;
        GameObject& AddGameObject();
        GameObject& ConsumeGameObject(GameObject&& gameObject);

        template <cDerivedComponent T, typename ...Args> T* AddComponent(ComponentOwnerKey, Args ...args)
        {
            return m_ComponentHandler.AddComponent<T>(args...);
        }
        void RemoveComponent(ComponentOwnerKey, const Component* pCompToRemove);
        template <template<typename> typename Container>
        void RemoveComponents(ComponentOwnerKey, const Container<Component*>& pCompsToRemove)
        {
            m_ComponentHandler.RemoveComponents<Container>(pCompsToRemove);
        }

        bool IsBeingDestroyed() const { return m_IsBeingDestroyed; }
    private:

        static constexpr std::size_t POOL_SIZE = 10'000;

        bool m_IsBeingDestroyed{false};
        BufferAllocator<POOL_SIZE> m_MemoryPool{};

        GameObjectHandler m_GameObjectHandler{m_MemoryPool};
        ComponentHandler m_ComponentHandler{m_MemoryPool};
    };


    // ---------------------------------------------------------------------------------------------------------------
    // TEMPLATED DEFINITONS

    // AnyComponent
    template <cDerivedComponent T, typename ... Args>
    Scene::ComponentHandler::AnyComponent Scene::ComponentHandler::AnyComponent::make_component(ComponentAllocator& alloc, Args... args)
    {
        return AnyComponent{new (alloc) T{args...}, alloc};
    }
    template <cDerivedComponent T>
    T* Scene::ComponentHandler::AnyComponent::Get() const
    {
        if (auto p = dynamic_cast<T*>(m_pComponent); p != nullptr)
            return p;

        throw std::bad_typeid();
    }
    template <cDerivedComponent T>
    Scene::ComponentHandler::AnyComponent::AnyComponent(T* component, ComponentAllocator& alloc):
            m_pComponent{component},
            m_Alloc{alloc}
    {}

    // ComponentHandler
    template <cDerivedComponent T, typename ... Args>
    T* Scene::ComponentHandler::AddComponent(Args... args)
    {
        const auto& typeID = typeid(T);
        if (!m_Allocs.contains(typeID))
        {
            auto [it, succeeded] = m_Allocs.try_emplace(typeID, std::type_identity<T>{}, m_rAlloc);
            if (!succeeded) throw std::runtime_error{std::format("Couldn't add typeID '{}'.", typeID.name())};
        }

        m_Components.emplace_back(AnyComponent::make_component<T>(m_Allocs.at(typeID), args...));
        auto typeComp = m_Components.back().Get<T>();
        typeComp->SetBufferIndex(BufferOwnerKey{}, m_Components.size() - 1);
        return typeComp;
    }
    template <template <typename> class Container>
    void Scene::ComponentHandler::RemoveComponents(const Container<Component*>& pCompsToRemove)
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
    T* Scene::ComponentHandler::GetComponent(std::size_t vecIndex)
    {
        return m_Components.at(vecIndex).Get<T>();
    }

    // ---------------------------------------------------------------------------------------------------------------
}

#endif //SCENE_H
