#include "Engine.h"
#include <thread>

#include "AudioService.h"

namespace jela
{
    Engine::Engine() :
        m_hInstance{nullptr},
        m_Title{ _T("Standard Game")},
        m_SecondsPerFrame{1.f/60.f}
    {}

    bool Engine::Init(HINSTANCE hInstance, const tstring& resourcePath, int width, int height, COLORREF bgColor, const tstring& wndwName)
    {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        // Use HeapSetInformation to specify that the process should terminate if the heap manager detects an error in any heap used by the process.
        // The return value is ignored, because we want to continue running in the unlikely event that HeapSetInformation fails.
        HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

        if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) return false;

        srand(static_cast<unsigned int>(time(nullptr)));
        m_hInstance = hInstance;

        m_Title = wndwName;
        SetFrameRate(60);

        try
        {
            m_pWindow = std::make_unique<GameWindow>(width, height, hInstance, m_Title, resourcePath,
                [pEngine = this](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
                {
                    return pEngine->MessageHandler(hWnd,message,wParam,lParam);
                }
            );

            m_pResourceManager = std::make_unique<ResourceManager>(resourcePath);
            m_pSceneManager = std::make_unique<SceneManager>();
            m_pRenderManager = std::make_unique<RenderManager>(m_pWindow.get());
            m_pInputManager = std::make_unique<InputManager>();

            m_pResourceManager->Start();
            m_pRenderManager->SetBackGroundColor(bgColor);
        }
        catch (...)
        {
            Shutdown();
            throw;
        }
        return true;
    }

    int Engine::Run()
    {
        m_pWindow->SetWindowPosition(true, true);

        LARGE_INTEGER countsPersSecond;
        LARGE_INTEGER currentCount;
        QueryPerformanceFrequency(&countsPersSecond);
        QueryPerformanceCounter(&currentCount);
        m_TriggerCount = currentCount;
        LARGE_INTEGER lastCount = currentCount;

        MSG msg{};
        bool playing = true;
        // Main message loop:
        while (playing)
        {
            QueryPerformanceCounter(&currentCount);
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    m_pWindow = nullptr;
                    playing = false;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (!playing) continue;


            if (m_IsVSyncEnabled || currentCount.QuadPart >= m_TriggerCount.QuadPart)
            {
                SetDeltaTime(static_cast<float>(currentCount.QuadPart - lastCount.QuadPart) / countsPersSecond.QuadPart);
                lastCount = currentCount;

                m_pInputManager->ProcessInput();
                m_pSceneManager->Update();
                m_pRenderManager->Render();

                m_TriggerCount.QuadPart = currentCount.QuadPart + static_cast<int>(m_SecondsPerFrame * countsPersSecond.QuadPart);
            }
        }

        return static_cast<int>(msg.wParam);
    }

    void Engine::Shutdown()
    {
        AudioLocator::RegisterAudioService(nullptr);

        m_pSceneManager = nullptr;
        m_pResourceManager = nullptr;
        m_pRenderManager = nullptr;
        m_pWindow = nullptr;

        CoUninitialize();
    }

    void Engine::Quit()
    {
        PostMessage(m_pWindow->GetWindow(), WM_DESTROY, NULL, NULL);
        m_IsQuitting = true;
    }

    GameWindow::MessageResult Engine::MessageHandler(HWND, UINT message, WPARAM wParam, LPARAM lParam)
    {
        GameWindow::MessageResult r{};
        switch (message)
        {
        case WM_ENTERSIZEMOVE:
        case WM_KILLFOCUS:
        case WM_EXITSIZEMOVE:
        case WM_SETFOCUS:
            QueryPerformanceCounter(&m_TriggerCount);
            r = 0;
            break;
        case WM_SIZE:
            m_pRenderManager->ResizeBackBuffer();
            r = 0;
            break;
        case WM_DISPLAYCHANGE:
            m_pRenderManager->Render();
            r = 0;
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            m_IsQuitting = true;
            r = 1;
            break;

        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_MOUSEWHEEL:
            m_pInputManager->QueueEvent({static_cast<WindowEvent>(message), wParam, lParam});
            r = 0;
            break;
        default:
            break;
        }

        return r;
    }

    void Engine::ShowMouse(bool show) const
    {
        m_pWindow->ShowMouse(show);
    }
    void Engine::UseSystemFramerate(bool enable)
    {
        m_IsVSyncEnabled = enable;
    }
    void Engine::SetFont(const Font* const pFont) const
    {
        m_pResourceManager->SetCurrentFont(pFont);
    }
    void Engine::SetTextFormat(TextFormat* const pTextFormat) const
    {
        m_pResourceManager->SetCurrentTextFormat(pTextFormat);
    }
    void Engine::SetInstance(HINSTANCE hInst)
    {
        m_hInstance = hInst;
    }
    void Engine::SetTitle(const tstring& newTitle)
    {
        m_Title.assign(newTitle);
        SetWindowText(m_pWindow->GetWindow(), newTitle.c_str());
    }
    void Engine::SetWindowDimensions(int width, int height, bool refreshWindowPos) const
    {
        m_pWindow->SetWindowDimensions(width, height, refreshWindowPos);
    }
    void Engine::SetWindowScale(float scale) const
    {
        m_pWindow->SetWindowScale(scale);
    }
    void Engine::SetDeltaTime(float elapsedSec)
    {
        m_DeltaTime = elapsedSec;
        m_TotalTime += elapsedSec;
    }
    void Engine::SetFrameRate(int FPS)
    {
        m_SecondsPerFrame = 1.f / FPS;
    }

    RenderManager* Engine::RenderMngr() const
    {
        return m_pRenderManager.get();
    }
    InputManager* Engine::InputMngr() const
    {
        return m_pInputManager.get();
    }
    ResourceManager* Engine::ResourceMngr() const
    {
        return m_pResourceManager.get();
    }
    SceneManager* Engine::SceneMngr() const
    {
        return m_pSceneManager.get();
    }
    const GameWindow* Engine::Window() const
    {
        return m_pWindow.get();
    }

    float Engine::GetDeltaTime() const
    {
        return m_DeltaTime;
    }
    float Engine::GetTotalTime() const
    {
        return m_TotalTime;
    }
    bool Engine::IsQuitting() const
    {
        return m_IsQuitting;
    }
    void Engine::NotifyError(HWND hWnd, const std::wstring& pszErrorMessage, HRESULT hrErr)
    {
        constexpr size_t MESSAGE_LEN = 512;
        TCHAR message[MESSAGE_LEN];

        if (SUCCEEDED(StringCchPrintf(message, MESSAGE_LEN, _T("%s (HRESULT = 0x%X)\n"),
            pszErrorMessage.c_str(), hrErr)))
        {
            MessageBox(hWnd, message, _T("ERROR"), MB_OK | MB_ICONERROR);
        }
        OutputDebugString(message);
    }
    void Engine::NotifyException(const std::string& exceptionMessage, const std::string& title) const
    {
        MessageBoxA(m_pWindow->GetWindow(), exceptionMessage.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
    }
}
