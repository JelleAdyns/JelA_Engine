// MyOwnEngineExercise.cpp : Defines the entry point for the application.
//
#include "Engine.h"

// Forward declarations of functions included in this code module:
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    ENGINE->SetInstance(hInstance);
    return ENGINE->Run(nCmdShow);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        ENGINE->SetWindow(hWnd);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT rect;
            HDC hdc = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rect);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


Engine* Engine::m_pEngine{ nullptr };

Engine::Engine(): 
    m_PaintColor{RGB(0,0,0)},
    m_pGame{ new Application{} },
    m_hInstance{NULL},
    m_hWindow{NULL},
    m_PaintHdc{ NULL },
    m_pTitle{ new tstring{L"Standard Game"}},
    m_Width{500},
    m_Height{500}
{

}

Engine* Engine::GetSingleton()
{
    if (m_pEngine == nullptr) m_pEngine = new Engine();
    return m_pEngine;
}

int Engine::Run(int nCmdShow)
{

    MSG msg;

    m_pGame->Initialize();
    MakeWindow(nCmdShow);
    // Main message loop:
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                DestroyWindow(m_hWindow);
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            PAINTSTRUCT ps;
            RECT rect;
            HDC hdc = BeginPaint(m_hWindow, &ps);
            GetClientRect(m_hWindow, &rect);

            m_PaintHdc = CreateCompatibleDC(hdc);
            HBITMAP bitmapBuffer = CreateCompatibleBitmap(hdc, m_Width, m_Height);

            HBITMAP bitmapOld = HBITMAP(SelectObject(m_PaintHdc, bitmapBuffer));
            // TODO: Add any drawing code that uses hdc here...
            m_pGame->Paint();
            m_pGame->Tick();
            PaintLine(POINT{ 23, 455 }, POINT{ 32, 5 });
            BitBlt(hdc, 0, 0, m_Width, m_Height, m_PaintHdc, 0, 0, SRCCOPY);

            SelectObject(m_PaintHdc, bitmapOld);
            DeleteObject(bitmapBuffer);

            DeleteDC(m_PaintHdc);
            EndPaint(m_hWindow, &ps);
        }
    }
    
    return (int)msg.wParam;
}
bool Engine::MakeWindow(int nCmdShow)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = m_pTitle->c_str();
    wcex.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_MYOWNENGINEEXERCISE));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    if(!RegisterClassEx(&wcex)) return false;

    m_hWindow = CreateWindowW(m_pTitle->c_str(), m_pTitle->c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInstance, nullptr);

    if (!m_hInstance) return false;

    ShowWindow(m_hWindow, nCmdShow);
    UpdateWindow(m_hWindow);

    return true;
}
void Engine::PaintLine(POINT first, POINT second)
{
    PaintLine(first, second, m_PaintHdc);
}
void Engine::PaintLine(POINT first, POINT second, HDC hDc)
{
    HPEN newPen = CreatePen(PS_SOLID, 2, m_PaintColor);
    HPEN oldPen = (HPEN)SelectObject(hDc, newPen);

    MoveToEx(hDc, first.x, first.y, 0);
    LineTo(hDc, second.x, second.y);
    MoveToEx(hDc, 0,0,0);

    SelectObject(hDc, oldPen);
    DeleteObject(newPen);
}
HINSTANCE Engine::GetInstance() const
{
    return m_hInstance;
}
void Engine::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
}
void Engine::SetTitle(const tstring& newTitle)
{
    m_pTitle->assign(newTitle);
}
void Engine::SetWindow(HWND hWindow)
{
    m_hWindow = hWindow;
}
void Engine::SetWindowDimensions(POINT windowDimensions)
{
    m_Width = windowDimensions.x;
    m_Height = windowDimensions.y;
}
void Engine::SetColor(COLORREF newColor)
{
    m_PaintColor = newColor;
}