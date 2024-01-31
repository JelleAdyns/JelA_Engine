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
    
    // Use HeapSetInformation to specify that the process should terminate if the heap manager detects an error in any heap used by the process.
    // The return value is ignored, because we want to continue running in the unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    ENGINE->SetInstance(HINST_THISCOMPONENT);
    return ENGINE->Run(nCmdShow);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ENGINE->HandleMessages(hWnd, message, wParam, lParam);
}


Engine* Engine::m_pEngine{ nullptr };

Engine::Engine() :
    m_hWindow{ NULL },
    m_hInstance{ NULL },
    m_pDFactory{ NULL },
    m_pDRenderTarget{NULL},
    m_pGame{ new Application{} },
    m_pDColorBrush{NULL},
    m_PaintColor{RGB(0,0,0)},
    m_PaintHdc{ NULL },
    m_pTitle{ new tstring{L"Standard Game"}},
    m_Width{500},
    m_Height{500}
{

}

Engine::~Engine()
{
    SafeRelease(&m_pDFactory);
    SafeRelease(&m_pDRenderTarget);
    SafeRelease(&m_pDColorBrush);
    delete m_pGame;
}

Engine* Engine::GetSingleton()
{
    if (m_pEngine == nullptr) m_pEngine = new Engine();
    return m_pEngine;
}
LRESULT Engine::HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        Application* pApplication = (Application*)pcs->lpCreateParams;

        ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApplication));
        // ENGINE->SetWindow(hWnd);
        result = 1;
    }
    else
    {
        bool wasHandled = false;
        if (m_pGame)
        {
            switch (message)
            {
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                if (m_pDRenderTarget)
                {
                    //If error occurs, it will be returned by EndDraw()
                    m_pDRenderTarget->Resize(D2D1::SizeU(width, height));
                }
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DISPLAYCHANGE:
            {
                InvalidateRect(hWnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_PAINT:
            {
                m_pDRenderTarget->BeginDraw();
                m_pDRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
                m_pDRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
                m_pGame->Paint();
                //m_pGame->Tick();
                ValidateRect(hWnd, NULL);
                m_pDRenderTarget->EndDraw();
               
                //PAINTSTRUCT ps;
                //RECT rect;
                //HDC hdc = BeginPaint(hWnd, &ps);
                //GetClientRect(hWnd, &rect);
                //m_PaintHdc = CreateCompatibleDC(hdc);
                //HBITMAP bitmapBuffer = CreateCompatibleBitmap(hdc, m_Width, m_Height);

                //HBITMAP bitmapOld = HBITMAP(SelectObject(m_PaintHdc, bitmapBuffer));
                //// TODO: Add any drawing code that uses hdc here...
                //
                //m_pGame->Paint();
                //ValidateRect(hWnd, NULL);
                //m_pGame->Tick();

                //BitBlt(hdc, 0, 0, m_Width, m_Height, m_PaintHdc, 0, 0, SRCCOPY);

                //SelectObject(m_PaintHdc, bitmapOld);
                //DeleteObject(bitmapBuffer);

                //DeleteDC(m_PaintHdc);
                //EndPaint(hWnd, &ps);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DESTROY:
                PostQuitMessage(0);
                result = 1;
                wasHandled = true;
                break;
            }     
        }
        if (!wasHandled)
        {
            result = DefWindowProc(hWnd, message, wParam, lParam);
        }
        
    }
    return result;
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
            m_pGame->Tick();
            InvalidateRect(m_hWindow, NULL, FALSE);
        }
    }
    
    return (int)msg.wParam;
}
HRESULT Engine::MakeWindow(int nCmdShow)
{
    HRESULT hr = S_OK;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDFactory);

    if (SUCCEEDED(hr))
    {
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = m_hInstance;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = m_pTitle->c_str();
        wcex.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_MYOWNENGINEEXERCISE));
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        RegisterClassEx(&wcex);

     
        m_hWindow = CreateWindowW(m_pTitle->c_str(), m_pTitle->c_str(), WS_OVERLAPPEDWINDOW,
            0, 0, NULL, NULL, nullptr, nullptr, m_hInstance, nullptr);
        CreateOurRenderTarget();
        if (m_hWindow)
        {
            ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

            int windowWidth{ m_Width + GetSystemMetrics(SM_CXBORDER) * 2 };
            int windowHeight{ m_Height + GetSystemMetrics(SM_CYBORDER) * 2 };

            int xPos{ GetSystemMetrics(SM_CXSCREEN) / 2 - windowWidth / 2 };
            int yPos{ GetSystemMetrics(SM_CYSCREEN) / 2 - windowHeight / 2 };

            
            SetWindowPos(m_hWindow, NULL, xPos, yPos, windowWidth, windowHeight, SWP_DRAWFRAME);

            ShowWindow(m_hWindow, nCmdShow);
            UpdateWindow(m_hWindow);
        }
    }

    return hr;
   
}
HRESULT Engine::CreateOurRenderTarget()
{
    HRESULT hr = S_OK;

    if (!m_pDRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hWindow, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        // Create a Direct2D render target.
        hr = m_pDFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hWindow, size),
            &m_pDRenderTarget
        );
    }

    return hr;
}
//void Engine::PaintLine(POINT first, POINT second)
//{
//    PaintLine(first, second, m_PaintHdc);
//}
void Engine::PaintLine(int firstX, int firstY, int secondX, int secondY, FLOAT lineThickness)
{
    m_pDRenderTarget->DrawLine(
        D2D1::Point2F(static_cast<FLOAT>(firstX), static_cast<FLOAT>(firstY)),
        D2D1::Point2F(static_cast<FLOAT>(secondX), static_cast<FLOAT>(secondY)),
        m_pDColorBrush,
        lineThickness
    );
   /* HPEN newPen = CreatePen(PS_SOLID, 2, m_PaintColor);
    HPEN oldPen = (HPEN)SelectObject(hDc, newPen);

    MoveToEx(hDc, first.x, first.y, 0);
    LineTo(hDc, second.x, second.y);
    MoveToEx(hDc, 0,0,0);

    SelectObject(hDc, oldPen);
    DeleteObject(newPen);*/
}
//HINSTANCE Engine::GetInstance() const
//{
//    return m_hInstance;
//}
void Engine::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
}
void Engine::SetTitle(const tstring& newTitle)
{
    m_pTitle->assign(newTitle);
}
//void Engine::SetWindow(HWND hWindow)
//{
//    m_hWindow = hWindow;
//}
void Engine::SetWindowDimensions(POINT windowDimensions)
{
    m_Width = windowDimensions.x;
    m_Height = windowDimensions.y;
}
void Engine::SetColor(COLORREF newColor, FLOAT opacity)
{
    if (m_pDColorBrush) SafeRelease(&m_pDColorBrush);
    m_pDRenderTarget->CreateSolidColorBrush(D2D1::ColorF(GetRValue(newColor), GetGValue(newColor), GetBValue(newColor), opacity), &m_pDColorBrush);
    m_PaintColor = newColor;
}