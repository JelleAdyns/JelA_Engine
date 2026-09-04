#include "SceneManager.h"

namespace jela
{
    void SceneManager::Draw() const
    {
        for (const auto& [render, pScene] : m_Scenes)
            if(render) pScene->Draw();
    }
    void SceneManager::Update()
    {
        if (Scene* pTopScene = m_Scenes.back().pScene.get();
            m_pActiveScene != pTopScene)
        {
            m_pActiveScene = pTopScene;
            if (m_pActiveScene) m_pActiveScene->Start();
        }

        if (m_pActiveScene) m_pActiveScene->Update();

    }
}
