#include "JelA/Managers/SceneManager.h"

namespace jela
{
    void SceneManager::Draw() const
    {
        if (!m_FirstUpdatePassed) return;

        for (const auto& [render, pScene] : m_Scenes)
            if(render) pScene->Draw();
    }
    void SceneManager::Update()
    {
        if (m_Scenes.empty()) return;

        if (m_SceneChanged)
        {
            m_pActiveScene = m_Scenes.back().pScene.get();
            if (m_pActiveScene) m_pActiveScene->Start();
            m_SceneChanged = false;
        }

        if (m_pActiveScene) m_pActiveScene->Update();

        m_FirstUpdatePassed = true;
    }
}
