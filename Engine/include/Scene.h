#ifndef SCENE_H
#define SCENE_H

#include "GameObject.h"

namespace jela
{
    class Scene final
    {
    private:
        // class GameObjectHandler final
        // {
        // public:
        //
        //     GameObjectHandler();
        //     ~GameObjectHandler() { Clear(); }
        //     GameObjectHandler(const GameObjectHandler& other) = delete;
        //     GameObjectHandler(GameObjectHandler&& other) noexcept = default;
        //     GameObjectHandler& operator=(const GameObjectHandler& other) = delete;
        //     GameObjectHandler& operator=(GameObjectHandler&& other) noexcept = delete;
        //
        //     GameObject& AddGameObject();
        //     GameObject& ConsumeGameObject(GameObject&& gameObject);
        //     void Clear();
        //
        //     static constexpr std::size_t MAX_GAME_OBJECTS = 100;
        // private:
        //
        //     FixedSizeAllocator m_GameObjectAlloc;
        //     std::vector<GameObject*> m_pGameObjects{};
        // };
    public:

        Scene() = default;



        // TODO: REFERENCE BECOMES INVALID WHEN VECTOR RESIZES, FIX THIS!!!
        GameObject& AddGameObject();
        GameObject& ConsumeGameObject(GameObject&& gameObject);

    private:

        //GameObjectHandler m_GameObjectHandler{};
        std::vector<GameObject> m_pGameObjects{};
    };


    // ---------------------------------------------------------------------------------------------------------------
}

#endif //SCENE_H
