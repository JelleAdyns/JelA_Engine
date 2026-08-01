#include "Scene.h"
#include "GameObject.h"

namespace jela
{

    Scene::~Scene()
    {
        m_GameObjectHandler.Clear();
        m_ComponentHandler.Clear();
    }
    GameObject& Scene::AddGameObject()
    {
        return std::forward<GameObject&>(m_GameObjectHandler.AddGameObject(*this));
    }
    GameObject& Scene::ConsumeGameObject(GameObject&& gameObject)
    {
        if (&(gameObject.GetScene()) != this)
            throw std::runtime_error{"Cannot consume GameObject because the GameObject has a reference to a different scene."};
        return std::forward<GameObject&>(m_GameObjectHandler.ConsumeGameObject(std::move(gameObject)));
    }
}
