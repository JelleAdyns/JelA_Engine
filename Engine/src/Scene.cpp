#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h"

namespace jela
{
    void Scene::Draw() const
    {
        for (const auto & obj : m_GameObjectHandler.GameObjects())
        {
            auto pos = obj->Transform()->WorldPosition();
            ENGINE.DrawEllipse(Point2f{pos.x, pos.y}, 10.f, 20.f);
        }
    }
    GameObject& Scene::AddGameObject()
    {
        return GameObject::Create(*this);
    }
    GameObject& Scene::ConsumeGameObject(GameObject&& gameObject)
    {
        return std::forward<GameObject&>(m_GameObjectHandler.ConsumeGameObject(std::move(gameObject)));
    }

    GameObject& Scene::GameObjectHandler::ConsumeGameObject(GameObject&& gameObject)
    {
        auto* p = static_cast<GameObject*>(m_GameObjectAlloc.Acquire(sizeof(GameObject)));
        p = new (p) GameObject{std::move(gameObject)};
        return *(m_pGameObjects.emplace_back(p));
    }
    void Scene::GameObjectHandler::Clear()
    {
        for (const auto pObject : m_pGameObjects)
        {
            std::destroy_at(pObject);
            operator delete(pObject, m_GameObjectAlloc);
        }
        m_pGameObjects.clear();
    }
}
