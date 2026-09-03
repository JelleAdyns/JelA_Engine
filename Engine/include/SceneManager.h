#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#include <unordered_map>

#include "Scene.h"
#include "Defines.h"

namespace jela
{
    class SceneManager final
    {
    public:
        Scene& CreateScene(const tstring& sceneName);

        void Start();
        void Draw() const;
        void Update();

    private:
        std::unordered_map<tstring, std::unique_ptr<Scene>> m_pScenes;
    };
}


#endif //SCENEMANAGER_H
