#include "GameWindow.h"

#include <windowsx.h>

#include <utility>

#include "Engine.h"

namespace jela
{
    GameWindow::GameWindow(int width, int height, HINSTANCE hInstance, const tstring& title, const tstring& dataPath, MessageHandler&& messageHandler):
        m_GameWidth{width},
        m_GameHeight{height},
        m_WindowWidth{width},
        m_WindowHeight{height},
        m_ViewPortWidth{static_cast<float>(width)},
        m_ViewPortHeight{static_cast<float>(height)},
        m_MessageHandler{std::move(messageHandler)}
    {
        WNDCLASSEX wcex{};

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = title.c_str();

        //https://stackoverflow.com/questions/77638632/how-to-add-an-icon-to-a-win32api-window-in-a-cmake-project
        const auto hIcon = static_cast<HICON>(LoadImage(
            hInstance,
            (dataPath + _T("WindowIcon.ico")).c_str(),
            IMAGE_ICON,
            GetSystemMetrics(SM_CXICON),
            GetSystemMetrics(SM_CYICON),
            LR_LOADFROMFILE | LR_DEFAULTSIZE));

        wcex.hIcon = hIcon;
        wcex.hIconSm = hIcon;

        RegisterClassEx(&wcex);
        
        m_hWindow = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, title.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, NULL, CW_USEDEFAULT, NULL, nullptr, nullptr, hInstance, this);
        if (m_hWindow == nullptr)
        {
            DWORD error = GetLastError();
            OutputDebugString((_T("CreateWindow failed: ") + to_tstring(error)).c_str());
        }
    }
    GameWindow::~GameWindow()
    {
        DestroyWindow(m_hWindow);
    }
    void GameWindow::SetWindowDimensions(int width, int height, bool refreshWindowPos)
    {
        m_GameWidth = width;
        m_GameHeight = height;
        SetWindowPosition(refreshWindowPos, true);
    }
    void GameWindow::SetWindowScale(float scale)
    {
        m_WindowScale = scale;
        SetWindowPosition(false,true);
    }
    void GameWindow::SetWindowPosition(bool setPos, bool setSize)
    {
        MONITORINFOEX mi{};
        mi.cbSize = sizeof(MONITORINFOEX);

        if (GetMonitorInfo(MonitorFromWindow(m_hWindow, MONITOR_DEFAULTTOPRIMARY), &mi))
        {
            ::SetWindowLongPtr(m_hWindow, GWL_STYLE, m_OriginalStyle);
            const LONG_PTR exStyle = ::GetWindowLongPtr(m_hWindow, GWL_EXSTYLE);
            const int width = static_cast<int>(m_GameWidth * m_WindowScale);
            const int height = static_cast<int>(m_GameHeight * m_WindowScale);
            const int xPos = mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left) / 2 - width / 2;
            const int yPos = mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2 - height / 2;

            RECT wr = {xPos,yPos,xPos + width,yPos + height };
            AdjustWindowRectExForDpi(&wr, m_OriginalStyle, FALSE, static_cast<DWORD>(exStyle), GetDpiForWindow(m_hWindow));

            if (setSize)
            {
                m_WindowWidth = wr.right - wr.left;
                m_WindowHeight = wr.bottom - wr.top;
            }
            if (setPos)
            {
                m_WindowPosX = wr.left;
                m_WindowPosY = wr.top;
            }

            ::SetWindowPos(m_hWindow, nullptr, m_WindowPosX, m_WindowPosY, m_WindowWidth, m_WindowHeight, SWP_FRAMECHANGED);

            m_IsFullscreen = false;
        }

        ::ShowWindow(m_hWindow, SW_SHOWNORMAL);
        ::UpdateWindow(m_hWindow);

    }
    void GameWindow::SetFullscreen()
    {
        MONITORINFOEX mi{};
        mi.cbSize = sizeof(MONITORINFOEX);

        if(GetMonitorInfo(MonitorFromWindow(m_hWindow, MONITOR_DEFAULTTONEAREST), &mi))
        {
            //https://www.codeproject.com/Questions/108400/How-to-Set-Win32-Application-to-Full-Screen-C
            m_OriginalStyle = static_cast<DWORD>(::GetWindowLongPtr(m_hWindow, GWL_STYLE));
            const auto ExStyle = static_cast<DWORD>(::GetWindowLongPtr(m_hWindow, GWL_EXSTYLE));
            constexpr DWORD dwRemove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
            const DWORD dwNewStyle = m_OriginalStyle & ~dwRemove;
            constexpr DWORD dwExRemove = WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;
            const DWORD dwExNewStyle = ExStyle & ~dwExRemove;
            ::SetWindowLongPtr(m_hWindow, GWL_STYLE, dwNewStyle);
            ::SetWindowLongPtr(m_hWindow, GWL_EXSTYLE, dwExNewStyle);

            m_WindowWidth = static_cast<int>(mi.rcMonitor.right - mi.rcMonitor.left);
            m_WindowHeight = static_cast<int>(mi.rcMonitor.bottom - mi.rcMonitor.top);

            m_WindowPosX = mi.rcMonitor.left;
            m_WindowPosY = mi.rcMonitor.top;

            ::SetWindowPos(m_hWindow, nullptr, m_WindowPosX, m_WindowPosY, m_WindowWidth, m_WindowHeight, SWP_FRAMECHANGED);
            m_IsFullscreen = true;
        }

        ShowWindow(m_hWindow, SW_DENORMAL);
        UpdateWindow(m_hWindow);
    }
    void GameWindow::CalculateWindowPos()
    {
        LPRECT lpRect{ new RECT{} };
        ::GetWindowRect(m_hWindow, lpRect);
        m_WindowPosX = lpRect->left;
        m_WindowPosY = lpRect->top;

        delete lpRect;
    }


    void GameWindow::ShowMouse(bool show) const
    {
        ShowCursor(show);

        InvalidateRect(m_hWindow, nullptr, true);
    }

    Vector2f GameWindow::GetGameSize() const
    {
        return { static_cast<float>(m_GameWidth), static_cast<float>(m_GameHeight) };
    }
    Point2f GameWindow::GetViewportPos() const
    {
        return Point2f{
            static_cast<float>(m_ViewPortTranslationX),
            static_cast<float>(m_ViewPortTranslationY)
        };
    }
    Vector2f GameWindow::GetViewportSize() const
    {
        return Vector2f{
            static_cast<float>(m_ViewPortWidth),
            static_cast<float>(m_ViewPortHeight)
        };
    }
    Point2f GameWindow::GetWindowPos() const
    {
        return Point2f{
            static_cast<float>(m_WindowPosX),
            static_cast<float>(m_WindowPosY)
        };
    }
    Vector2f GameWindow::GetWindowSize() const
    {
        return Vector2f{
            static_cast<float>(m_WindowWidth),
            static_cast<float>(m_WindowHeight)
        };
    }
    float GameWindow::GetWindowScale() const
    {
        return m_WindowScale;
    }
    float GameWindow::GetWindowPosOffset() const
    {
        const UINT dpi = GetDpiForWindow(m_hWindow);
        const float dpiMultiplier = dpi / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
        return m_WindowPosOffset * dpiMultiplier;
    }
    HWND GameWindow::GetWindow() const
    {
        return m_hWindow;
    }
    bool GameWindow::IsWindowActive() const
    {
        return m_WindowIsActive;
    }
    bool GameWindow::IsFullscreen() const
    {
        return m_IsFullscreen;
    }

    LRESULT CALLBACK GameWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        GameWindow *pThis{};

        if (message == WM_NCCREATE)
        {
            pThis = static_cast<GameWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

            SetLastError(0);
            if (!SetWindowLongPtr(hWnd, GWLA_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
            {
                if (GetLastError() != 0) return FALSE;
            }
        }
        else pThis = reinterpret_cast<GameWindow*>(GetWindowLongPtr(hWnd, GWLA_USERDATA));

        if (pThis) return pThis->HandleMessages(hWnd, message, wParam, lParam).value();

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    GameWindow::MessageResult GameWindow::HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        MessageResult result{};

        switch (message)
        {
        case WM_ACTIVATE:
            {
                if (LOWORD(wParam) == WA_INACTIVE) m_WindowIsActive = false;
                else if (LOWORD(wParam) == WA_CLICKACTIVE || LOWORD(wParam) == WA_ACTIVE) m_WindowIsActive = true;
            }
            result = 0;
            break;

        case WM_SIZE:
            {
                const UINT width = LOWORD(lParam);
                const UINT height = HIWORD(lParam);

                m_WindowWidth = static_cast<int>(width);
                m_WindowHeight = static_cast<int>(height);

                m_ViewPortWidth = m_GameWidth * m_WindowScale;
                m_ViewPortHeight = m_GameHeight * m_WindowScale;

                m_MinScale = std::min<float>(
                        m_WindowWidth / m_ViewPortWidth,
                        m_WindowHeight / m_ViewPortHeight
                    );
                m_ViewPortWidth *= m_MinScale;
                m_ViewPortHeight *= m_MinScale;

                m_ViewPortTranslationX = (m_WindowWidth - m_ViewPortWidth) / 2.f;
                m_ViewPortTranslationY = (m_WindowHeight - m_ViewPortHeight) / 2.f;

                CalculateWindowPos();
            }
            result = 0;
            break;

        case WM_MOVE:
            CalculateWindowPos();
            result = 0;
            break;

        case WM_KEYUP:
            {
                if (static_cast<int>(wParam) == VK_F11)
                {
                    if (m_IsFullscreen) SetWindowPosition(true, true);
                    else SetFullscreen();
                }
            }
            result = 0;
            break;
        default:
            break;
        }

        result = m_MessageHandler(hWnd, message, wParam, lParam);

        if (!result.has_value())
            result = DefWindowProc(hWnd, message, wParam, lParam);

        return result;
    }
} // jela