
#include "Engine.h"
#include <chrono>
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
    m_pDColorBrush{NULL},
    m_pGame{ new Application{} },
    m_pTitle{ new tstring{L"Standard Game"}},
    m_Width{500},
    m_Height{500},
    m_TimePerFrame{1.f/60.f}
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
            case WM_GETMINMAXINFO: {
               /* DWORD dwStyle = ::GetWindowLong(m_hWindow, GWL_STYLE);
                DWORD dwRemove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
                DWORD dwNewStyle = dwStyle & ~dwRemove;
                ::SetWindowLong(m_hWindow, GWL_STYLE, dwNewStyle);
                ::SetWindowPos(m_hWindow, NULL, 0, 0, 0, 0, SWP_NOSIZE
                    | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
                HDC hDC = ::GetWindowDC(NULL);
                ::SetWindowPos(m_hWindow, NULL, 0, 0, ::GetDeviceCaps(hDC, HORZRES), ::GetDeviceCaps(hDC, VERTRES), SWP_FRAMECHANGED);*/

                //return 0;
            }
            result = 0;
            wasHandled = false;
            break;
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
                /* DWORD dwStyle = ::GetWindowLong(m_hWindow, GWL_STYLE);
                DWORD dwRemove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
                DWORD dwNewStyle = dwStyle & ~dwRemove;
                ::SetWindowLong(m_hWindow, GWL_STYLE, dwNewStyle);
                ::SetWindowPos(m_hWindow, NULL, 0, 0, 0, 0, SWP_NOSIZE
                    | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
                HDC hDC = ::GetWindowDC(NULL);
                ::SetWindowPos(m_hWindow, NULL, 0, 0,
                ::GetDeviceCaps(hDC, HORZRES),
                ::GetDeviceCaps(hDC, VERTRES),
                SWP_FRAMECHANGED);*/
                InvalidateRect(hWnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_PAINT:
            {
                HRESULT hr = S_OK;

                CreateOurRenderTarget();
                m_pDRenderTarget->BeginDraw();

                // Clear background
                m_pDRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
                m_pDRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

                // Set tranformation for when the window changes in size
                // The user draw calls should always appear in the middle of the screen,
                // not the left corner
                int rendertargetWidth{ GetRenderTargetSize().width };
                int rendertargetHeight{ GetRenderTargetSize().height };

                float scaleX{ rendertargetWidth / static_cast<float>(m_Width) };
                float scaleY{ rendertargetHeight / static_cast<float>(m_Height) };
                FLOAT minScale{ std::min<float>(scaleX,scaleY) };

                FLOAT translationX{ (rendertargetWidth - m_Width * minScale) / 2.f };
                FLOAT translationY{ (rendertargetHeight - m_Height * minScale) / 2.f };

                m_pDRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(minScale, minScale) *
                    D2D1::Matrix3x2F::Translation(D2D1::SizeF(translationX, translationY)));

                // User Draw Calls
                m_pGame->Draw();

                // Dont show more than the the scaled window size given by the user
                m_pDRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
                SetColor(RGB(0, 0, 0));
                DrawBorders(rendertargetWidth, rendertargetHeight, translationX, translationY);

                m_pDRenderTarget->EndDraw();

                if (hr == D2DERR_RECREATE_TARGET)
                {
                    hr = S_OK;
                    SafeRelease(&m_pDRenderTarget);
                    SafeRelease(&m_pDColorBrush);
                }
                ValidateRect(hWnd, NULL);
               
                //PAINTSTRUCT ps;
                //RECT rect;
                //HDC hdc = BeginPaint(hWnd, &ps);
                //GetClientRect(hWnd, &rect);
                //m_PaintHdc = CreateCompatibleDC(hdc);
                //HBITMAP bitmapBuffer = CreateCompatibleBitmap(hdc, m_Width, m_Height);
                //
                //HBITMAP bitmapOld = HBITMAP(SelectObject(m_PaintHdc, bitmapBuffer));
                //// TODO: Add any drawing code that uses hdc here...
                //
                //m_pGame->Paint();
                //ValidateRect(hWnd, NULL);
                //m_pGame->Tick();
                //
                //BitBlt(hdc, 0, 0, m_Width, m_Height, m_PaintHdc, 0, 0, SRCCOPY);
                //
                //SelectObject(m_PaintHdc, bitmapOld);
                //DeleteObject(bitmapBuffer);
                //
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

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    double elapsedSec{};
   
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
            std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

            elapsedSec += std::chrono::duration<double>(t2- t1).count();
            
            t1 = t2;

            while (elapsedSec > m_TimePerFrame)
            {
                m_pGame->Tick();
                elapsedSec -= m_TimePerFrame;
            }
            
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

        m_hWindow = CreateWindow(m_pTitle->c_str(), m_pTitle->c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, nullptr, nullptr, m_hInstance, this);


        if (m_hWindow)
        {
            //https://www.codeproject.com/Questions/108400/How-to-Set-Win32-Application-to-Full-Screen-C
            /*DWORD dwStyle = ::GetWindowLong(m_hWindow, GWL_STYLE);
            DWORD dwRemove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
            DWORD dwNewStyle = dwStyle & ~dwRemove;
            ::SetWindowLong(m_hWindow, GWL_STYLE, dwNewStyle);
            ::SetWindowPos(m_hWindow, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE
                | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            HDC hDC = ::GetWindowDC(NULL);
            ::SetWindowPos(m_hWindow, NULL, 0, 0,
            ::GetDeviceCaps(hDC, HORZRES),
            ::GetDeviceCaps(hDC, VERTRES),
            SWP_FRAMECHANGED);*/
            UINT dpi = GetDpiForWindow(m_hWindow);

            int windowWidth{ (GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + m_Width +10) };
            int windowHeight{ (GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
                                GetSystemMetrics(SM_CYCAPTION) + m_Height+10) };

            windowWidth = static_cast<int>(windowWidth * dpi / 96.f);
            windowHeight = static_cast<int>(windowHeight * dpi / 96.f);

            int xPos{ GetSystemMetrics(SM_CXSCREEN) / 2 - windowWidth / 2 };
            int yPos{ GetSystemMetrics(SM_CYSCREEN) / 2 - windowHeight / 2 };

            //::SetWindowLong(m_hWindow, GWL_STYLE, GetWindowLong(m_hWindow, GWL_STYLE) & ~WS_SIZEBOX);
            //HDC hDC = ::GetWindowDC(NULL);
            //::SetWindowPos(m_hWindow, NULL, 0, 0, ::GetDeviceCaps(hDC, HORZRES), ::GetDeviceCaps(hDC, VERTRES), SWP_FRAMECHANGED);
            ::SetWindowPos(m_hWindow, NULL, xPos, yPos, windowWidth, windowHeight, SWP_ASYNCWINDOWPOS);

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

        if (!m_pDColorBrush)
        {
            SetColor(RGB(255, 255, 255));
        }
    }

    return hr;
}
//void Engine::PaintLine(POINT first, POINT second)
//{
//    PaintLine(first, second, m_PaintHdc);
//}
void Engine::DrawBorders(int rtWidth, int rtHeight, FLOAT translationX, FLOAT translationY) const
{


    int reserveSpace{ 5 };
    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(translationX - 1),
            static_cast<FLOAT>(rtHeight + reserveSpace)),
        m_pDColorBrush);

    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(rtWidth - translationX),
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(rtWidth + reserveSpace),
            static_cast<FLOAT>(rtHeight + reserveSpace)),
        m_pDColorBrush);

    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(rtWidth + reserveSpace),
            static_cast<FLOAT>(translationY - 1)),
        m_pDColorBrush);

    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(rtHeight - translationY + 1),
            static_cast<FLOAT>(rtWidth + reserveSpace),
            static_cast<FLOAT>(rtHeight + reserveSpace)),
        m_pDColorBrush);
}
void Engine::DrawLine(int firstX, int firstY, int secondX, int secondY, float lineThickness)const
{
    m_pDRenderTarget->DrawLine(
        D2D1::Point2F(static_cast<FLOAT>(firstX), static_cast<FLOAT>(firstY)),
        D2D1::Point2F(static_cast<FLOAT>(secondX), static_cast<FLOAT>(secondY)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness)
    );
   /* HPEN newPen = CreatePen(PS_SOLID, 2, m_PaintColor);
    HPEN oldPen = (HPEN)SelectObject(hDc, newPen);

    MoveToEx(hDc, first.x, first.y, 0);
    LineTo(hDc, second.x, second.y);
    MoveToEx(hDc, 0,0,0);

    SelectObject(hDc, oldPen);
    DeleteObject(newPen);*/
}
void Engine::DrawRectangle(int left, int top, int width, int height, float lineThickness)const
{
    m_pDRenderTarget->DrawRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(left),
            static_cast<FLOAT>(top),
            static_cast<FLOAT>(left + width),
            static_cast<FLOAT>(top + height)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}
void Engine::DrawRoundedRect(int left, int top, int width, int height, float radius, float lineThickness)const
{
    DrawRoundedRect(left, top, width, height, radius, radius, lineThickness);
}
void Engine::DrawRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY, float lineThickness)const
{
    m_pDRenderTarget->DrawRoundedRectangle(
        D2D1::RoundedRect(
            D2D1::RectF(
                static_cast<FLOAT>(left),
                static_cast<FLOAT>(top),
                static_cast<FLOAT>(left + width),
                static_cast<FLOAT>(top + height)),
            static_cast<FLOAT>(radiusX),
            static_cast<FLOAT>(radiusY)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}
void Engine::DrawEllipse(int centerX, int centerY, int radius, float lineThickness)const
{
    DrawEllipse(centerX, centerY, radius, radius, lineThickness);
}
void Engine::DrawEllipse(int centerX, int centerY, int radiusX, int radiusY, float lineThickness)const
{
    m_pDRenderTarget->DrawEllipse(
        D2D1::Ellipse(
            D2D1::Point2F(static_cast<FLOAT>(centerX), static_cast<FLOAT>(centerY)),
            static_cast<FLOAT>(radiusX),
            static_cast<FLOAT>(radiusY)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}
void Engine::DrawString(int left, int top, int width, int height)const
{

}
void Engine::DrawBitmap(int left, int top, int width, int height)const
{

}
void Engine::FillRectangle(int left, int top, int width, int height)const
{
    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(left),
            static_cast<FLOAT>(top),
            static_cast<FLOAT>(left + width),
            static_cast<FLOAT>(top + height)),
        m_pDColorBrush);
}
void Engine::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
}
void Engine::SetTitle(const tstring& newTitle)
{
    m_pTitle->assign(newTitle);
}
void Engine::SetWindowDimensions(int width, int height)
{
    m_Width = width;
    m_Height = height;
}
void Engine::SetFrameRate(int FPS)
{
    m_TimePerFrame = 1.0 / FPS;
}
void Engine::SetColor(COLORREF newColor, float opacity)
{
    if (m_pDColorBrush) SafeRelease(&m_pDColorBrush);
    m_pDRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF(
            GetRValue(newColor)/255.f,
            GetGValue(newColor)/255.f,
            GetBValue(newColor)/255.f,
            static_cast<FLOAT>(opacity))),
        &m_pDColorBrush);
}

RectInt Engine::GetRenderTargetSize() const
{
    D2D1_SIZE_F size = m_pDRenderTarget->GetSize();
    return RectInt{ 0,0,static_cast<int>(size.width),static_cast<int>(size.height)};
}
RectInt Engine::GetWindowSize() const
{
    return RectInt{ 0, 0, m_Width, m_Height };
}