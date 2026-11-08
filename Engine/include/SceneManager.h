#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "Defines.h"

namespace jela
{
    template <typename T>
    concept cDerivedScene = std::is_base_of_v<Scene, T> || !std::is_same_v<T, Scene>;
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

        template <cDerivedScene SceneType>
        Scene* SetScene()
        {
            while (!m_Scenes.empty())
            {
                m_Scenes.back().pScene->OnExit();
                m_Scenes.pop_back();
            }

            return AddScene<SceneType>();
        }

        template <cDerivedScene SceneType>
        Scene* PushScene(bool keepRenderingPrevScene)
        {
            if (!m_Scenes.empty())
            {
                m_Scenes.back().pScene->OnSuspend();
                m_Scenes.back().render = keepRenderingPrevScene;
            }

            return AddScene<SceneType>();
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

            m_SceneChanged = true;
        }
    private:

        template <cDerivedScene SceneType>
        Scene* AddScene()
        {
            Scene* pCreatedScene = m_Scenes.emplace_back(true, std::make_unique<SceneType>()).pScene.get();

            pCreatedScene->Load();        // Adding all necessary objects to the scene with their components
            pCreatedScene->InitObjects(); // Initializing the components of the objects
            pCreatedScene->OnEnter();     // Final actions the user might want to perform after all objects are completely initialized

            m_SceneChanged = true;

            return pCreatedScene;
        }
        struct SceneInfo
        {
            bool render;
            std::unique_ptr<Scene> pScene;
        };

        bool m_SceneChanged{true};
        bool m_FirstUpdatePassed{false};
        Scene* m_pActiveScene {nullptr};
        std::vector<SceneInfo> m_Scenes{}; // Technicaly used as a std::stack, but using std::vector to be able to iterate over all scenes in "Draw()"
    };
}


#endif //SCENEMANAGER_H
