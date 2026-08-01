#include "Scene.h"
#include "GameObject.h"

namespace jela
{
    GameObject& Scene::AddGameObject()
    {
        return m_pGameObjects.emplace_back();
        //return std::forward<GameObject&>(m_GameObjectHandler.AddGameObject(*this));
    }
    GameObject& Scene::ConsumeGameObject(GameObject&& gameObject)
    {
        return m_pGameObjects.emplace_back(std::move(gameObject));
        //return std::forward<GameObject&>(m_GameObjectHandler.ConsumeGameObject(std::move(gameObject)));
    }

    // Scene::GameObjectHandler::GameObjectHandler():
    //     m_GameObjectAlloc{std::type_identity<GameObject>{}, MAX_GAME_OBJECTS}
    // {
    // }
    // GameObject& Scene::GameObjectHandler::AddGameObject()
    // {
    //     m_pGameObjects.emplace_back(new (m_GameObjectAlloc) GameObject{});
    //     return *m_pGameObjects.back();
    // }
    // GameObject& Scene::GameObjectHandler::ConsumeGameObject(GameObject&& gameObject)
    // {
    //     auto* p = static_cast<GameObject*>(m_GameObjectAlloc.Acquire(sizeof(GameObject)));
    //     p = new (p) GameObject{std::move(gameObject)};
    //     return *(m_pGameObjects.emplace_back(p));
    // }
    // void Scene::GameObjectHandler::Clear()
    // {
    //     for (const auto pObject : m_pGameObjects)
    //     {
    //         pObject->~GameObject();
    //         operator delete(pObject, m_GameObjectAlloc);
    //     }
    //     m_pGameObjects.clear();
    // }
}
