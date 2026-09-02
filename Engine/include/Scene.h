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

            GameObjectHandler() = default;
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

            TypeAllocator<GameObject, MAX_GAME_OBJECTS> m_GameObjectAlloc{};
            std::vector<GameObject*> m_pGameObjects{};
        };
    public:

        static constexpr std::size_t GetMaxObjects() { return GameObjectHandler::MAX_GAME_OBJECTS; };

        void Draw() const;
        GameObject& AddGameObject();
        GameObject& ConsumeGameObject(GameObject&& gameObject);

    private:

        GameObjectHandler m_GameObjectHandler{};
    };
}

#endif //SCENE_H
