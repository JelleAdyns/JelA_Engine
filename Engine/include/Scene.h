#ifndef SCENE_H
#define SCENE_H

#include "ComponentHandler.h"
#include "GameObjectHandler.h"

namespace jela
{
    class Scene final
    {
    public:

        Scene() = default;

        ~Scene();

        GameObject& AddGameObject();
        GameObject& ConsumeGameObject(GameObject&& gameObject);

        template <cDerivedComponent T, typename ...Args>
        size_t AddComponent(Args ...args) { return m_ComponentHandler.AddComponent<T>(args...); }

        void RemoveComponent(size_t vecIndex) { m_ComponentHandler.RemoveComponent(vecIndex); }

        template <cDerivedComponent T>
        T* GetComponent(size_t vecIndex) { return m_ComponentHandler.GetComponent<T>(vecIndex); }

    private:

        ComponentHandler m_ComponentHandler{};
        GameObjectHandler m_GameObjectHandler{};

    };


    // ---------------------------------------------------------------------------------------------------------------
}

#endif //SCENE_H
