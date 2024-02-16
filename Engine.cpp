
#include "Engine.h"
#include "Game.h"
#include <chrono>

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // Use HeapSetInformation to specify that the process should terminate if the heap manager detects an error in any heap used by the process.
    // The return value is ignored, because we want to continue running in the unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        ENGINE->SetInstance(HINST_THISCOMPONENT);
        return ENGINE->Run();
        CoUninitialize();
    }
    return 0;
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
    m_DColorBackGround{ D2D1::ColorF(D2D1::ColorF::Black)},
    m_pGame{ nullptr },
    m_pTitle{ tstring{L"Standard Game"}},
    m_Width{500},
    m_Height{500},
    m_TimePerFrame{1.f/60.f},
    m_IsFullscreen{false},
    m_KeyIsDown{false}
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
        BaseGame* pBaseGame = (BaseGame*)pcs->lpCreateParams;

        ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pBaseGame));
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
                HRESULT hr = OnRender();

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
            case WM_KEYUP:
            {
                if (static_cast<int>(wParam) == VK_F11)
                {
                    if (m_IsFullscreen) SetWindowPosition();
                    else SetFullscreen();
                    m_IsFullscreen = !m_IsFullscreen;
                }
                
                m_pGame->KeyUp(static_cast<int>(wParam));
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_KEYDOWN:
            {
                m_pGame->KeyDown(static_cast<int>(wParam));
            }
            result = 0;
            wasHandled = true;
            break;


            case WM_LBUTTONDOWN:
                m_pGame->MouseDown(true, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                result = 0;
                wasHandled = true;
                break;
            case WM_LBUTTONUP:
                m_pGame->MouseUp(true, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                result = 0;
                wasHandled = true;
                break;
            case WM_RBUTTONDOWN:
                m_pGame->MouseDown(false, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                result = 0;
                wasHandled = true;
                break;
            case WM_RBUTTONUP:
                m_pGame->MouseUp(false, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                result = 0;
                wasHandled = true;
                break;
            case WM_MOUSEMOVE:
                m_pGame->MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
                result = 0;
                wasHandled = true;
                break;
            case WM_MOUSEWHEEL:
                m_pGame->MouseWheelTurn(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), GET_WHEEL_DELTA_WPARAM(wParam), LOWORD(wParam));
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

int Engine::Run()
{

    MSG msg;
    MakeWindow();       
    CreateOurRenderTarget(); // ALWAYS CREATE RENDERTARGET BEFORE CALLING CONSTRUCTOR OF pGAME.
                             // TEXTURES ARE CREATED IN THE CONSTRUCTOR AND THEY NEED THE RENDERTARGET. 
    m_pGame = new Game{};
    m_pGame->Initialize();

    

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    float elapsedSec{};
   
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

            elapsedSec += std::chrono::duration<float>(t2 - t1).count();
            
            t1 = t2;

            while (elapsedSec > m_TimePerFrame)
            {
                m_pGame->Tick(m_TimePerFrame);
                elapsedSec -= m_TimePerFrame;
            }
            InvalidateRect(m_hWindow, NULL, FALSE);
            
           
        }
    }
    
    return (int)msg.wParam;
}
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
HRESULT Engine::MakeWindow()
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
        wcex.lpszClassName = m_pTitle.c_str();
        wcex.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_MYOWNENGINEEXERCISE));
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        RegisterClassEx(&wcex);

        m_hWindow = CreateWindow(m_pTitle.c_str(), m_pTitle.c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, nullptr, nullptr, m_hInstance, this);

        if (m_hWindow) SetWindowPosition();
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
HRESULT Engine::OnRender()
{
    HRESULT hr = S_OK;

    CreateOurRenderTarget();
    m_pDRenderTarget->BeginDraw();

    // Clear background
    m_pDRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    m_pDRenderTarget->Clear(m_DColorBackGround);

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
    
    hr = m_pDRenderTarget->EndDraw();
    return hr;
}


#ifdef MATHEMATICAL_COORDINATESYSTEM
//lines
void Engine::DrawLine(int firstX, int firstY, int secondX, int secondY, float lineThickness)const
{
    DrawLine(Point2Int{ firstX,firstY }, Point2Int{ secondX, secondY }, lineThickness);
}
void Engine::DrawLine(const Point2Int& firstPoint, const Point2Int& secondPoint, float lineThickness) const
{
    m_pDRenderTarget->DrawLine(
        D2D1::Point2F(static_cast<FLOAT>(firstPoint.x), static_cast<FLOAT>(m_Height - firstPoint.y)),
        D2D1::Point2F(static_cast<FLOAT>(secondPoint.x), static_cast<FLOAT>(m_Height - secondPoint.y)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness)
    );
}

//Rectangles
void Engine::DrawRectangle(int left, int bottom, int width, int height, float lineThickness)const
{
    DrawRectangle(RectInt{ left, bottom , width, height}, lineThickness);
}
void Engine::DrawRectangle(const Point2Int& leftBottom, int width, int height, float lineThickness)const
{
    DrawRectangle(RectInt{ leftBottom, width, height }, lineThickness);
}
void Engine::DrawRectangle(const RectInt& rect, float lineThickness)const
{
    m_pDRenderTarget->DrawRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(rect.left),
            static_cast<FLOAT>(m_Height - (rect.bottom + rect.height)),
            static_cast<FLOAT>(rect.left + rect.width),
            static_cast<FLOAT>(m_Height - rect.bottom)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}

//RoundedRects
void Engine::DrawRoundedRect(int left, int bottom, int width, int height, float radiusX, float radiusY, float lineThickness)const
{
    DrawRoundedRect(RectInt{ left, bottom, width, height }, radiusX, radiusY, lineThickness);
}
void Engine::DrawRoundedRect(const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY, float lineThickness)const
{
    DrawRoundedRect(RectInt{ leftBottom, width, height }, radiusX, radiusY, lineThickness);
}
void Engine::DrawRoundedRect(const RectInt& rect, float radiusX, float radiusY, float lineThickness)const
{
    m_pDRenderTarget->DrawRoundedRectangle(
        D2D1::RoundedRect(
            D2D1::RectF(
                static_cast<FLOAT>(rect.left),
                static_cast<FLOAT>(m_Height - (rect.bottom + rect.height)),
                static_cast<FLOAT>(rect.left + rect.width),
                static_cast<FLOAT>(m_Height - rect.bottom)),
            static_cast<FLOAT>(radiusX),
            static_cast<FLOAT>(radiusY)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}

//Ellipses
void Engine::DrawEllipse(int centerX, int centerY, int radiusX, int radiusY, float lineThickness)const
{
    DrawEllipse(EllipseInt{ centerX, centerY, radiusX, radiusY }, lineThickness);
}
void Engine::DrawEllipse(const Point2Int& center, int radiusX, int radiusY, float lineThickness)const
{
    DrawEllipse(EllipseInt{ center, radiusX, radiusY }, lineThickness);
}
void Engine::DrawEllipse(const EllipseInt& ellipse, float lineThickness)const
{
    m_pDRenderTarget->DrawEllipse(
        D2D1::Ellipse(
            D2D1::Point2F(static_cast<FLOAT>(ellipse.center.x), static_cast<FLOAT>(m_Height - ellipse.center.y)),
            static_cast<FLOAT>(ellipse.radiusX),
            static_cast<FLOAT>(ellipse.radiusY)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}

// Strings
void Engine::DrawString(const tstring& textToDisplay, Font* font, int left, int bottom, int width, int height, bool showRect)const
{
    DrawString(textToDisplay, font, RectInt{ left, bottom, width, height }, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, Font* font, Point2Int leftBottom, int width, int height, bool showRect)const
{
    DrawString(textToDisplay, font, RectInt{ leftBottom.x, leftBottom.y, width, height }, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, Font* font, RectInt destRect, bool showRect)const
{
    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<FLOAT>(destRect.left),
        static_cast<FLOAT>(m_Height - (destRect.bottom + destRect.height)),
        static_cast<FLOAT>(destRect.left + destRect.width),
        static_cast<FLOAT>(m_Height - destRect.bottom));

    if (showRect)
    {
        m_pDRenderTarget->DrawRectangle(rect, m_pDColorBrush);
    }

    m_pDRenderTarget->DrawText(
        textToDisplay.c_str(),
        textToDisplay.length(),
        font->GetFormat(),
        rect,
        m_pDColorBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);
}

void Engine::DrawString(const tstring& textToDisplay, Font* font, int left, int bottom, int width, bool showRect)const
{
    DrawString(textToDisplay, font, Point2Int{ left, bottom }, width, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, Font* font, Point2Int leftBottom, int width, bool showRect)const
{
    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<FLOAT>(leftBottom.x),
        static_cast<FLOAT>(m_Height - (leftBottom.y + font->GetFontSize())),
        static_cast<FLOAT>(leftBottom.x + width),
        static_cast<FLOAT>(m_Height - leftBottom.y));
    if (showRect)
    {
        m_pDRenderTarget->DrawRectangle(rect, m_pDColorBrush);
    }
    
    m_pDRenderTarget->DrawText(
        textToDisplay.c_str(),
        textToDisplay.length(),
        font->GetFormat(),
        rect,
        m_pDColorBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);
}
//-----------------
//Fill
//----------------

//Rectangles
void Engine::FillRectangle(int left, int bottom, int width, int height)const
{
    FillRectangle(RectInt{ left, bottom , width, height });
}
void Engine::FillRectangle(const Point2Int& leftBottom, int width, int height)const
{
    FillRectangle(RectInt{ leftBottom, width, height });
}
void Engine::FillRectangle(const RectInt& rect)const
{
    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(rect.left),
            static_cast<FLOAT>(m_Height - (rect.bottom + rect.height)),
            static_cast<FLOAT>(rect.left + rect.width),
            static_cast<FLOAT>(m_Height - rect.bottom)),
        m_pDColorBrush);
}

//RoundedRects
void Engine::FillRoundedRect(int left, int bottom, int width, int height, float radiusX, float radiusY)const
{
    FillRoundedRect(RectInt{ left, bottom, width, height }, radiusX, radiusY);
}
void Engine::FillRoundedRect(const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY)const
{
    FillRoundedRect(RectInt{ leftBottom, width, height }, radiusX, radiusY);
}
void Engine::FillRoundedRect(const RectInt& rect, float radiusX, float radiusY)const
{
    m_pDRenderTarget->FillRoundedRectangle(
        D2D1::RoundedRect(
            D2D1::RectF(
                static_cast<FLOAT>(rect.left),
                static_cast<FLOAT>(m_Height - (rect.bottom + rect.height)),
                static_cast<FLOAT>(rect.left + rect.width),
                static_cast<FLOAT>(m_Height - rect.bottom)),
            static_cast<FLOAT>(radiusX),
            static_cast<FLOAT>(radiusY)),
        m_pDColorBrush);
}

//Ellipses
void Engine::FillEllipse(int centerX, int centerY, int radiusX, int radiusY)const
{
    FillEllipse(EllipseInt{ centerX, centerY, radiusX, radiusY });
}
void Engine::FillEllipse(const Point2Int& center, int radiusX, int radiusY)const
{
    FillEllipse(EllipseInt{ center, radiusX, radiusY });
}
void Engine::FillEllipse(const EllipseInt& ellipse)const
{
    m_pDRenderTarget->FillEllipse(
        D2D1::Ellipse(
            D2D1::Point2F(static_cast<FLOAT>(ellipse.center.x), static_cast<FLOAT>(m_Height - ellipse.center.y)),
            static_cast<FLOAT>(ellipse.radiusX),
            static_cast<FLOAT>(ellipse.radiusY)),
        m_pDColorBrush);
}
#else

//Lines
void Engine::DrawLine(int firstX, int firstY, int secondX, int secondY, float lineThickness)const
{
    DrawLine(Point2Int{ firstX, firstY }, Point2Int{ secondX, secondY }, lineThickness);
}
void Engine::DrawLine(const Point2Int& firstPoint, const Point2Int& secondPoint, float lineThickness) const
{
    m_pDRenderTarget->DrawLine(
        D2D1::Point2F(static_cast<FLOAT>(firstPoint.x), static_cast<FLOAT>(firstPoint.y)),
        D2D1::Point2F(static_cast<FLOAT>(secondPoint.x), static_cast<FLOAT>(secondPoint.y)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness)
    );
}

//Rectangles
void Engine::DrawRectangle(int left, int top, int width, int height, float lineThickness)const
{
    DrawRectangle(RectInt{ left,top,width,height }, lineThickness);
}
void Engine::DrawRectangle(const Point2Int& leftTop, int width, int height, float lineThickness)const
{
    DrawRectangle(RectInt{ leftTop, width, height }, lineThickness);
}
void Engine::DrawRectangle(const RectInt& rect, float lineThickness)const
{
    m_pDRenderTarget->DrawRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(rect.left),
            static_cast<FLOAT>(rect.top),
            static_cast<FLOAT>(rect.left + rect.width),
            static_cast<FLOAT>(rect.top + rect.height)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}

//RoundedRects
void Engine::DrawRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY, float lineThickness)const
{
    DrawRoundedRect(RectInt{ left, top, width, height }, radiusX, radiusY, lineThickness);
}
void Engine::DrawRoundedRect(const Point2Int& leftTop, int width, int height, float radiusX, float radiusY, float lineThickness)const
{
    DrawRoundedRect(RectInt{ leftTop, width, height }, radiusX, radiusY, lineThickness);
}
void Engine::DrawRoundedRect(const RectInt& rect, float radiusX, float radiusY, float lineThickness)const
{
    m_pDRenderTarget->DrawRoundedRectangle(
        D2D1::RoundedRect(
            D2D1::RectF(
                static_cast<FLOAT>(rect.left),
                static_cast<FLOAT>(rect.top),
                static_cast<FLOAT>(rect.left + rect.width),
                static_cast<FLOAT>(rect.top + rect.height)),
            static_cast<FLOAT>(radiusX),
            static_cast<FLOAT>(radiusY)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}

//Ellipse
void Engine::DrawEllipse(int centerX, int centerY, int radiusX, int radiusY, float lineThickness)const
{
    DrawEllipse(EllipseInt{ centerX, centerY, radiusX, radiusY }, lineThickness);
}
void Engine::DrawEllipse(const Point2Int& center, int radiusX, int radiusY, float lineThickness)const
{
    DrawEllipse(EllipseInt{ center, radiusX, radiusY }, lineThickness);
}
void Engine::DrawEllipse(const EllipseInt& ellipse, float lineThickness)const
{
    m_pDRenderTarget->DrawEllipse(
        D2D1::Ellipse(
            D2D1::Point2F(static_cast<FLOAT>(ellipse.center.x), static_cast<FLOAT>(ellipse.center.y)),
            static_cast<FLOAT>(ellipse.radiusX),
            static_cast<FLOAT>(ellipse.radiusY)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}
void Engine::DrawString(const tstring& textToDisplay, Font* font, int left, int top, int width, int height, bool showRect)const
{
    DrawString(textToDisplay, font, RectInt{ left, top, width, height }, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, Font* font, Point2Int leftTop, int width, int height, bool showRect)const
{
    DrawString(textToDisplay, font, RectInt{ leftTop.x, leftTop.y, width, height }, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, Font* font, RectInt destRect, bool showRect)const
{
    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<FLOAT>(destRect.left),
        static_cast<FLOAT>(destRect.top),
        static_cast<FLOAT>(destRect.left + destRect.width),
        static_cast<FLOAT>(destRect.top + destRect.height));

    if (showRect)
    {
        m_pDRenderTarget->DrawRectangle(rect, m_pDColorBrush);
    }

    m_pDRenderTarget->DrawText(
        textToDisplay.c_str(),
        textToDisplay.length(),
        font->GetFormat(),
        rect,
        m_pDColorBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);
}

//Takes the size of the font as Height of the destination rectangle in order to have a logical position
void Engine::DrawString(const tstring& textToDisplay, Font* font, int left, int top, int width, bool showRect)const
{
    DrawString(textToDisplay, font, Point2Int{ left,top }, width, showRect);
}
//Takes the size of the font as Height of the destination rectangle in order to have a logical position
void Engine::DrawString(const tstring& textToDisplay, Font* font, Point2Int leftTop, int width, bool showRect)const
{
    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<FLOAT>(leftTop.x),
        static_cast<FLOAT>(leftTop.y),
        static_cast<FLOAT>(leftTop.x + width),
        static_cast<FLOAT>(leftTop.y + font->GetFontSize()));

    if (showRect)
    {
        m_pDRenderTarget->DrawRectangle(rect, m_pDColorBrush);
    }
    
    m_pDRenderTarget->DrawText(
        textToDisplay.c_str(),
        textToDisplay.length(),
        font->GetFormat(),
        rect,
        m_pDColorBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);
}
//-----------------
//Fill
//-----------------

//Rectangles
void Engine::FillRectangle(int left, int top, int width, int height)const
{
    FillRectangle(RectInt{ left,top,width,height });
}
void Engine::FillRectangle(const Point2Int& leftTop, int width, int height)const
{
    FillRectangle(RectInt{ leftTop, width, height });
}
void Engine::FillRectangle(const RectInt& rect)const
{
    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(rect.left),
            static_cast<FLOAT>(rect.top),
            static_cast<FLOAT>(rect.left + rect.width),
            static_cast<FLOAT>(rect.top + rect.height)),
        m_pDColorBrush);
}

//RoundedRects
void Engine::FillRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY)const
{
    FillRoundedRect(RectInt{ left, top, width, height }, radiusX, radiusY);
}
void Engine::FillRoundedRect(const Point2Int& leftTop, int width, int height, float radiusX, float radiusY)const
{
    FillRoundedRect(RectInt{ leftTop, width, height }, radiusX, radiusY);
}
void Engine::FillRoundedRect(const RectInt& rect, float radiusX, float radiusY)const
{
    m_pDRenderTarget->FillRoundedRectangle(
        D2D1::RoundedRect(
            D2D1::RectF(
                static_cast<FLOAT>(rect.left),
                static_cast<FLOAT>(rect.top),
                static_cast<FLOAT>(rect.left + rect.width),
                static_cast<FLOAT>(rect.top + rect.height)),
            static_cast<FLOAT>(radiusX),
            static_cast<FLOAT>(radiusY)),
        m_pDColorBrush);
}

//Ellipse
void Engine::FillEllipse(int centerX, int centerY, int radiusX, int radiusY)const
{
    FillEllipse(EllipseInt{ centerX, centerY, radiusX, radiusY });
}
void Engine::FillEllipse(const Point2Int& center, int radiusX, int radiusY)const
{
    FillEllipse(EllipseInt{ center, radiusX, radiusY });
}
void Engine::FillEllipse(const EllipseInt& ellipse)const
{
    m_pDRenderTarget->FillEllipse(
        D2D1::Ellipse(
            D2D1::Point2F(static_cast<FLOAT>(ellipse.center.x), static_cast<FLOAT>(ellipse.center.y)),
            static_cast<FLOAT>(ellipse.radiusX),
            static_cast<FLOAT>(ellipse.radiusY)),
        m_pDColorBrush);
}
#endif // MATHEMATICAL_COORDINATSYSTEM

bool Engine::IsKeyPressed(int virtualKeycode) const
{
    return GetKeyState(virtualKeycode) < 0;
}
void Engine::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
}
void Engine::SetTitle(const tstring& newTitle)
{
    m_pTitle.assign(newTitle);
    SetWindowText(m_hWindow, newTitle.c_str());
}
void Engine::SetWindowDimensions(int width, int height)
{
    m_Width = width;
    m_Height = height;

    SetWindowPosition();
}
void Engine::SetWindowPosition()
{
    DWORD dwAdd = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
    ::SetWindowLong(m_hWindow, GWL_STYLE, dwAdd);

    UINT dpi = GetDpiForWindow(m_hWindow);

    int windowWidth{ (GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + m_Width + 10) };
    int windowHeight{ (GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
                        GetSystemMetrics(SM_CYCAPTION) + m_Height + 10) };

    windowWidth = static_cast<int>(windowWidth * dpi / 96.f);
    windowHeight = static_cast<int>(windowHeight * dpi / 96.f);

    int xPos{ GetSystemMetrics(SM_CXSCREEN) / 2 - windowWidth / 2 };
    int yPos{ GetSystemMetrics(SM_CYSCREEN) / 2 - windowHeight / 2 };

    ::SetWindowPos(m_hWindow, NULL, xPos, yPos, windowWidth, windowHeight, SWP_FRAMECHANGED);
    ShowWindow(m_hWindow, SW_SHOWNORMAL);
    UpdateWindow(m_hWindow);
    
}
void Engine::SetFullscreen()
{
    //https://www.codeproject.com/Questions/108400/How-to-Set-Win32-Application-to-Full-Screen-C
    DWORD dwStyle = ::GetWindowLong(m_hWindow, GWL_STYLE);
    DWORD dwRemove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
    DWORD dwNewStyle = dwStyle & ~dwRemove;
    ::SetWindowLong(m_hWindow, GWL_STYLE, dwNewStyle);
    ::SetWindowPos(m_hWindow, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE
        | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    HDC hDC = ::GetWindowDC(m_hWindow);
    ::SetWindowPos(m_hWindow, NULL, 0, 0,
        ::GetDeviceCaps(hDC, HORZRES),
        ::GetDeviceCaps(hDC, VERTRES),
        SWP_FRAMECHANGED);
    ShowWindow(m_hWindow, SW_SHOWNORMAL);
    UpdateWindow(m_hWindow);
}
void Engine::SetFrameRate(int FPS)
{
    m_TimePerFrame = 1.0f / FPS;
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
void Engine::SetBackGroundColor(COLORREF newColor)
{
    m_DColorBackGround = D2D1::ColorF(
        GetRValue(newColor) / 255.f,
        GetGValue(newColor) / 255.f,
        GetBValue(newColor) / 255.f,
        1.f);
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
ID2D1HwndRenderTarget* Engine::getRenderTarget() const
{
    return m_pDRenderTarget;
}


//---------------------
//TEXTURE
//---------------------

IWICImagingFactory* Texture::m_pWICFactory{ nullptr };

Texture::Texture(const tstring& filename):
    m_pDBitmap{NULL},
    m_TextureWidth{0},
    m_TextureHeight{0}
{

    HRESULT creationResult = S_OK;

    if (!m_pWICFactory)
    {
        creationResult = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pWICFactory)
        );
    }

    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICFormatConverter* pConverter = NULL;

    if (SUCCEEDED(creationResult))
    {
        creationResult = m_pWICFactory->CreateDecoderFromFilename(
            filename.c_str(),
            NULL,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder);
    }


    if (SUCCEEDED(creationResult))
    {
        // Create the initial frame.
        creationResult = pDecoder->GetFrame(0, &pSource);
    }


    // Convert the image format to 32bppPBGRA
 // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    if (SUCCEEDED(creationResult)) creationResult = m_pWICFactory->CreateFormatConverter(&pConverter);
    if (SUCCEEDED(creationResult))
    {
        creationResult = pConverter->Initialize(
            pSource,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut
        );
    }


    if (SUCCEEDED(creationResult))
    {
        creationResult = ENGINE->getRenderTarget()->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            &m_pDBitmap
        );


        if (SUCCEEDED(creationResult))
        {
            m_TextureWidth = m_pDBitmap->GetSize().width;
            m_TextureHeight = m_pDBitmap->GetSize().height;
        }
    }
 
    if (!SUCCEEDED(creationResult))
    {
        tstring message = _T("ERROR! File \"") + filename + _T("\" couldn't load correctly");
        OutputDebugString(message.c_str());
    }
    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pConverter);

}
Texture::~Texture()
{
    SafeRelease(&m_pDBitmap);
};

#ifdef MATHEMATICAL_COORDINATESYSTEM
void Texture::DrawTexture(int destLeft, int destBottom, const RectInt& srcRect, float opacity)const
{
    DrawTexture(RectInt{ destLeft, destBottom, int(m_TextureWidth), int(m_TextureHeight) }, srcRect, opacity);
}
void Texture::DrawTexture(const Point2Int& destLeftBottom, const RectInt& srcRect, float opacity)const
{
    DrawTexture(RectInt{ destLeftBottom.x, destLeftBottom.y, int(m_TextureWidth), int(m_TextureHeight) }, srcRect, opacity);
}
void Texture::DrawTexture(const RectInt& destRect, const RectInt& srcRect, float opacity)const
{
    RectInt wndwSize = ENGINE->GetWindowSize();

    D2D1_RECT_F destination = D2D1::RectF(
        static_cast<FLOAT>(destRect.left),//0
        static_cast<FLOAT>(wndwSize.height - (destRect.bottom + destRect.height)),//screenheight
        static_cast<FLOAT>(destRect.left + destRect.width), //destination width
        static_cast<FLOAT>(wndwSize.height - destRect.bottom)
    );
    D2D1_RECT_F source{};
    if (srcRect.width <= 0 || srcRect.height <= 0)
    {
        source = D2D1::RectF(
            static_cast<FLOAT>(destRect.left),
            static_cast<FLOAT>(destRect.bottom),
            static_cast<FLOAT>(destRect.left + destRect.width),
            static_cast<FLOAT>(destRect.bottom + destRect.height)
        );
    }
    else
    {
        source = D2D1::RectF(
            static_cast<FLOAT>(srcRect.left),
            static_cast<FLOAT>(srcRect.bottom),
            static_cast<FLOAT>(srcRect.left + srcRect.width),
            static_cast<FLOAT>(srcRect.bottom + srcRect.height));
        destination.right = destination.left + srcRect.width;
        destination.top = destination.bottom - srcRect.height;
    }

    ENGINE->getRenderTarget()->DrawBitmap(
        m_pDBitmap,
        destination,
        opacity,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        source
    );
}
#else
//Textures
void Texture::DrawTexture(int destLeft, int destTop, const RectInt& srcRect, float opacity)const
{
    DrawTexture(RectInt{ destLeft, destTop, int(m_TextureWidth), int(m_TextureHeight) }, srcRect, opacity);
}
void Texture::DrawTexture(const Point2Int& destLeftTop, const RectInt& srcRect, float opacity)const
{
    DrawTexture(RectInt{ destLeftTop.x, destLeftTop.y, int(m_TextureWidth), int(m_TextureHeight) }, srcRect, opacity);
}
void Texture::DrawTexture(const RectInt& destRect, const RectInt& srcRect, float opacity)const
{
    D2D1_RECT_F destination = D2D1::RectF(
        static_cast<FLOAT>(destRect.left),
        static_cast<FLOAT>(destRect.top),
        static_cast<FLOAT>(destRect.left + destRect.width),
        static_cast<FLOAT>(destRect.top + destRect.height)
    );

    D2D1_RECT_F source{};
    if (srcRect.width <= 0 || srcRect.height <= 0)
    {
        source = D2D1::RectF(
            static_cast<FLOAT>(destRect.left),
            static_cast<FLOAT>(destRect.top),
            static_cast<FLOAT>(destRect.left + destRect.width),
            static_cast<FLOAT>(destRect.top + destRect.height)
        );
    }
    else
    {
        source = D2D1::RectF(
            static_cast<FLOAT>(srcRect.left),
            static_cast<FLOAT>(srcRect.top),
            static_cast<FLOAT>(srcRect.left + srcRect.width),
            static_cast<FLOAT>(srcRect.top + srcRect.height));
        destination.right = destination.left + srcRect.width;
        destination.bottom = destination.top + srcRect.height;
    }
    ENGINE->getRenderTarget()->DrawBitmap(
        m_pDBitmap,
        destination,
        opacity,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        source
    );
}
#endif // MATHEMATICAL_COORDINATESYSTEM

//---------------------
//Font
//---------------------

IDWriteFactory5* Font::m_pDWriteFactory{ nullptr };

Font::Font(const std::wstring& fontName, bool fromFile)
{
    if (fromFile)
    {
        HRESULT hr = Initialize(fontName);
        if (SUCCEEDED(hr))
        {
            SetTextFormat(20, false, false);
        }
    }
    else
    {
        if (!m_pDWriteFactory)
        {
            DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory5),
                reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
        }
        m_FontName = fontName;
        SetTextFormat(20, false, false);
    }
    
}
Font::Font(const std::wstring& fontName, int size ,bool bold, bool italic, bool fromFile)
{
    if (fromFile)
    {
        HRESULT hr = Initialize(fontName);
        if (SUCCEEDED(hr))
        {
            SetTextFormat(size, bold, italic);
        }
    }
    else
    {
        if (!m_pDWriteFactory)
        {
            DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory5),
                reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
        }
        m_FontName = fontName;
        SetTextFormat(size, bold, italic);
    }
   
}
Font::~Font()
{
    SafeRelease(&m_pFontCollection);
    SafeRelease(&m_pTextFormat);
}
HRESULT Font::Initialize(const std::wstring& fontName)
{
    HRESULT hr = S_OK;

    IDWriteFontSetBuilder1* pFontSetBuilder{ nullptr };
    IDWriteFontSet* pFontSet{ nullptr };
    IDWriteFontFile* pFontFile{ nullptr };
    m_pFontCollection = nullptr;

    if (!m_pDWriteFactory)
    {
        DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory5),
            reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
    }


    hr = m_pDWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);
    if (SUCCEEDED(hr))
    {
        hr = m_pDWriteFactory->CreateFontFileReference(fontName.c_str(), NULL, &pFontFile);
    }
    if (SUCCEEDED(hr))
    {
        hr = pFontSetBuilder->AddFontFile(pFontFile);
    }
    if (SUCCEEDED(hr))
    {
        hr = pFontSetBuilder->CreateFontSet(&pFontSet);
    }
    if (SUCCEEDED(hr))
    {
        hr = m_pDWriteFactory->CreateFontCollectionFromFontSet(pFontSet, &m_pFontCollection);
    }

    IDWriteFontFamily* pFontFamily{ nullptr };
    IDWriteLocalizedStrings* pStrings{ nullptr };

    UINT32 length;
    std::wstring name{};

    if (SUCCEEDED(hr))
    {
        hr = m_pFontCollection->GetFontFamily(0, &pFontFamily);
    }
    if (SUCCEEDED(hr))
    {
        hr = pFontFamily->GetFamilyNames(&pStrings);
    }
    if (SUCCEEDED(hr))
    {
        hr = pStrings->GetStringLength(0, &length);
    }

    if (SUCCEEDED(hr))
    {
        name.resize(length);
        hr = pStrings->GetString(0, &name[0], length + 1);
    }


    if (!SUCCEEDED(hr))
    {
        OutputDebugString((_T("Something went wrong in the Font constructor using file ") + fontName).c_str());
    }
    else
    {
        m_FontName = name;

    }

    SafeRelease(&pFontSetBuilder);
    SafeRelease(&pFontSet);
    SafeRelease(&pFontFile);
    SafeRelease(&pFontFamily);
    SafeRelease(&pStrings);

    return hr;
}

void Font::SetTextFormat(int size, bool bold, bool italic)
{
    if (m_pTextFormat) SafeRelease(&m_pTextFormat);
    m_pDWriteFactory->CreateTextFormat(
        m_FontName.c_str(),
        m_pFontCollection,
        bold ? DWRITE_FONT_WEIGHT_EXTRA_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
        italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        static_cast<FLOAT>(size),
        L"en-us",
        &m_pTextFormat);

    m_FontSize = size;
}
IDWriteTextFormat* Font::GetFormat() const
{
    return m_pTextFormat;
}
int Font::GetFontSize() const
{
    return m_FontSize;
}