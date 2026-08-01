#ifndef GAMEOBJECTHANDLER_H
#define GAMEOBJECTHANDLER_H

#include <vector>
#include "FixedSizeAllocators.h"

namespace jela
{
    class GameObject;
    class Scene;
    class GameObjectHandler final
    {
    public:

        GameObjectHandler();
        ~GameObjectHandler() { Clear(); }
        GameObjectHandler(const GameObjectHandler& other) = delete;
        GameObjectHandler(GameObjectHandler&& other) noexcept = default;
        GameObjectHandler& operator=(const GameObjectHandler& other) = delete;
        GameObjectHandler& operator=(GameObjectHandler&& other) noexcept = delete;

        GameObject& AddGameObject(Scene& scene);
        GameObject& ConsumeGameObject(GameObject&& gameObject);
        void Clear();

    private:
        static constexpr std::size_t MAX_GAME_OBJECTS = 100;

        FixedSizeAllocator m_GameObjectAlloc;
        std::vector<GameObject*> m_pGameObjects{};
    };

}
#endif //GAMEOBJECTHANDLER_H
