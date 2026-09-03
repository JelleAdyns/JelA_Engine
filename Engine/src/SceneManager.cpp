#include "SceneManager.h"

namespace jela
{
    Scene& SceneManager::CreateScene(const std::wstring& sceneName)
    {
        if (m_pScenes.contains(sceneName)) throw std::runtime_error("Scene already exists!");
        m_pScenes[sceneName] = std::make_unique<Scene>();
        return *m_pScenes[sceneName];
    }
    void SceneManager::Start()
    {
        for (const auto & pScene : m_pScenes | std::views::values)
            pScene->Start();
    }
    void SceneManager::Draw() const
    {
        for (const auto & pScene : m_pScenes | std::views::values)
            pScene->Draw();
    }
    void SceneManager::Update()
    {
        for (const auto & pScene : m_pScenes | std::views::values)
            pScene->Update();
    }
}
