#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "Defines.h"

namespace jela
{
    template <typename T>
    concept cIsScene = std::is_base_of_v<Scene, T> || std::is_same_v<T, Scene>;
    class SceneManager final
    {
    public:

        SceneManager() = default;
        ~SceneManager() = default;
        SceneManager(const SceneManager&) = delete;
        SceneManager(SceneManager&&) noexcept = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager& operator=(SceneManager&&) noexcept = delete;

        void Draw() const;
        void Update();

        template <cIsScene SceneType>
        Scene* SetScene()
        {
            while (!m_Scenes.empty())
            {
                m_Scenes.back().pScene->OnExit();
                m_Scenes.pop_back();
            }

            Scene* pCreatedScene = m_Scenes.emplace_back(true, std::make_unique<SceneType>()).pScene.get();
            pCreatedScene->OnEnter();

            return pCreatedScene;
        }

        template <cIsScene SceneType>
        Scene* PushScene(bool keepRenderingPrevScene)
        {
            if (!m_Scenes.empty())
            {
                m_Scenes.back().pScene->OnSuspend();
                m_Scenes.back().render = keepRenderingPrevScene;
            }
            Scene* pCreatedScene = m_Scenes.emplace_back(true, std::make_unique<SceneType>()).pScene.get();
            pCreatedScene->OnEnter();

            return pCreatedScene;
        }

        void PopScene()
        {
            if (m_Scenes.empty()) return;

            m_Scenes.back().pScene->OnExit();
            m_Scenes.pop_back();

            if(!m_Scenes.empty())
            {
                m_Scenes.back().pScene->OnResume();
                m_Scenes.back().render = true;
            }
        }
    private:

        struct SceneInfo
        {
            bool render;
            std::unique_ptr<Scene> pScene;
        };

        Scene* m_pActiveScene {nullptr};
        std::vector<SceneInfo> m_Scenes{}; // Technicaly used as a std::stack, but using std::vector to be able to iterate over all scenes in "Draw()"
    };
}


#endif //SCENEMANAGER_H
