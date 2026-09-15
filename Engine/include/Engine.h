#ifndef ENGINE_H
#define ENGINE_H

#include "BaseGame.h"
#include "Structs.h"
#include "framework.h"
#include "Controller.h"
#include <vector>
#include <strsafe.h>

#include "ResourceManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "SceneManager.h"


namespace jela
{
    inline constexpr bool USE_MATHEMATICAL_COORDINATESYSTEM {
#ifdef MATHEMATICAL_COORDINATESYSTEM
        true
#else
        false
#endif
    };



    class Engine final
    {
    public:
        Engine();
        Engine(const Engine& other) = delete;
        Engine(Engine&& other) noexcept = delete;
        Engine& operator=(const Engine& other) = delete;
        Engine& operator=(Engine&& other) noexcept = delete;

        ~Engine() = default;

        bool Init(HINSTANCE hInstance, const tstring& resourcePath, int width, int height, COLORREF bgColor = RGB(0,0,0), const tstring& wndwName = _T("Game"));
        int Run();
        void Shutdown();
        void Quit();

        // Setters
        void ShowMouse(bool show) const;
        void UseSystemFramerate(bool enable);
        void SetFont(const Font* pFont) const;
        void SetTextFormat(TextFormat* pTextFormat) const;
        void SetInstance(HINSTANCE hInst);
        void SetTitle(const tstring& newTitle);
        void SetWindowDimensions(int width, int height, bool refreshWindowPos = true) const;
        void SetWindowScale(float scale) const;
        void SetFrameRate(int FPS);

        // Getters
        RenderManager* RenderMngr() const;
        InputManager* InputMngr() const;
        ResourceManager* ResourceMngr() const;
        SceneManager* SceneMngr() const;
        const GameWindow* Window() const;


        float GetDeltaTime() const;
        float GetTotalTime() const;
        bool IsQuitting() const;

        static void NotifyError(HWND hWnd, const tstring& pszErrorMessage, HRESULT hrErr);
        void NotifyException(const std::string& exceptionMessage, const std::string& title = "ERROR") const;

    private:

        void SetDeltaTime(float elapsedSec);
        GameWindow::MessageResult MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        //Win32
        HINSTANCE                       m_hInstance;
        LARGE_INTEGER                   m_TriggerCount{};

        //BaseGame
        std::unique_ptr<BaseGame>       m_pGame{};

        //General datamembers
        tstring                         m_Title{};

        float                           m_SecondsPerFrame{};
        float                           m_DeltaTime{};
        float                           m_TotalTime{};

        bool                            m_IsVSyncEnabled{true};
        bool                            m_IsQuitting{false};

        std::vector<std::unique_ptr<Controller>> m_pVecControllers{};

        std::unique_ptr<GameWindow> m_pWindow{};
        std::unique_ptr<RenderManager> m_pRenderManager{};
        std::unique_ptr<ResourceManager> m_pResourceManager{};
        std::unique_ptr<SceneManager> m_pSceneManager{};
        std::unique_ptr<InputManager> m_pInputManager{};
    };
}

// Extern declaration of the Engine global
extern jela::Engine ENGINE;

#endif // !ENGINE_H