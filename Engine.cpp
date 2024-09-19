
#include "Engine.h"
#include "Game.h"
#include <chrono>
#include <thread>
#include <algorithm>



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ENGINE.HandleMessages(hWnd, message, wParam, lParam);
}

Engine::Engine() :
    m_hWindow{ NULL },
    m_hInstance{ NULL },
    m_pDFactory{ NULL },
    m_pDRenderTarget{NULL},
    m_pDColorBrush{NULL},
    m_DColorBackGround{ D2D1::ColorF::Black},
    m_pGame{ nullptr },
    m_Title{ _T("Standard Game")},
    m_Width{500},
    m_Height{500},
    m_MilliSecondsPerFrame{1.f/60.f},
    m_IsFullscreen{false},
    m_KeyIsDown{false}
{
    
}

Engine::~Engine()
{
    delete m_pGame;
    SafeRelease(&m_pDFactory);
    SafeRelease(&m_pDRenderTarget);
    SafeRelease(&m_pDColorBrush);

}

Engine& Engine::GetSingleton()
{
    static std::unique_ptr<Engine> pEngine = std::unique_ptr<Engine>(new Engine{});
    return *pEngine;
}
LRESULT Engine::HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        BaseGame* pBaseGame = (BaseGame*)pcs->lpCreateParams;

        ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pBaseGame));
        // ENGINE.SetWindow(hWnd);
        result = 1;
    }
    else
    {
    
        bool wasHandled = false;
        if (m_pGame)
        {
            switch (message)
            {
            case WM_ENTERSIZEMOVE:
            case WM_KILLFOCUS:
            case WM_EXITSIZEMOVE:
            case WM_SETFOCUS:
            {
                m_T1 = std::chrono::high_resolution_clock::now();
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_ACTIVATE:
            {
                if (LOWORD(wParam) == WA_INACTIVE) m_WindowIsActive = false;
                else if (LOWORD(wParam) == WA_CLICKACTIVE || LOWORD(wParam) == WA_ACTIVE) m_WindowIsActive = true;
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_SIZE:
            {              
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                if (m_pDRenderTarget)
                {
                    //If error occurs, it will be returned by EndDraw()
                    m_pDRenderTarget->Resize(D2D1::SizeU(width, height));

                    int rendertargetWidth{ GetRenderTargetSize().width };
                    int rendertargetHeight{ GetRenderTargetSize().height };

                    float scaleX{ rendertargetWidth / (m_Width * m_WindowScale) };
                    float scaleY{ rendertargetHeight / (m_Height * m_WindowScale) };
                    float minScale{ std::min<float>(scaleX,scaleY) };

                    float translationX{ (rendertargetWidth - (m_Width * m_WindowScale) * minScale) / 2.f };
                    float translationY{ (rendertargetHeight - (m_Height * m_WindowScale) * minScale) / 2.f };

                    m_ViewPortTranslationX = translationX;
                    m_ViewPortTranslationY = translationY;
                    m_ViewPortScaling = minScale;
                    m_TransformChanged = true;
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
                Paint();
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
                m_pGame->MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), static_cast<int>(wParam));
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

    MakeWindow();       
    CreateRenderTarget(); // ALWAYS CREATE RENDERTARGET BEFORE CALLING CONSTRUCTOR OF pGAME.
                             // TEXTURES ARE CREATED IN THE CONSTRUCTOR AND THEY NEED THE RENDERTARGET. 

    srand(static_cast<unsigned int>(time(nullptr)));

    m_pGame = new Game{};
    m_pGame->Initialize();

    SetWindowPosition();

    MSG msg;
    m_T1 = std::chrono::high_resolution_clock::now();
    
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
            const std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

            float elapsedSec{ std::chrono::duration<float>(t2 - m_T1).count() };

            ENGINE.SetDeltaTime(elapsedSec);

            m_T1 = t2;

            m_pGame->Tick();
            Paint();

            const auto sleepTime = t2 + std::chrono::milliseconds(static_cast<int>(m_MilliSecondsPerFrame)) - std::chrono::high_resolution_clock::now();
            std::this_thread::sleep_for(sleepTime);
  
        }
    }
    
    m_pGame->Destroy();

    return (int)msg.wParam;
}
void Engine::DrawBorders(int rtWidth, int rtHeight) const
{

     
    int reserveSpace{ 5 };
    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(m_ViewPortTranslationX),
            static_cast<FLOAT>(rtHeight + reserveSpace)),
        m_pDColorBrush);

    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(rtWidth - m_ViewPortTranslationX),
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(rtWidth + reserveSpace),
            static_cast<FLOAT>(rtHeight + reserveSpace)),
        m_pDColorBrush);

    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(rtWidth + reserveSpace),
            static_cast<FLOAT>(m_ViewPortTranslationY)),
        m_pDColorBrush);

    m_pDRenderTarget->FillRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(-reserveSpace),
            static_cast<FLOAT>(rtHeight - m_ViewPortTranslationY),
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
        wcex.lpszClassName = m_Title.c_str();
        wcex.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_MYOWNENGINEEXERCISE));
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        RegisterClassEx(&wcex);

        m_hWindow = CreateWindow(m_Title.c_str(), m_Title.c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, nullptr, nullptr, m_hInstance, this);

        if (m_hWindow) SetWindowPosition();
    }

    return hr;
   
}
HRESULT Engine::CreateRenderTarget()
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
            m_pDRenderTarget->CreateSolidColorBrush(D2D1::ColorF(255, 255, 255), &m_pDColorBrush);
        }
    }

    return hr;
}
HRESULT Engine::OnRender()
{
    HRESULT hr = S_OK;

    CreateRenderTarget();
    m_pDRenderTarget->BeginDraw();

    // Clear background
    //m_pDRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    m_pDRenderTarget->Clear(m_DColorBackGround);

    // Set tranformation for when the window changes in size
    // The user draw calls should always appear in the middle of the screen,
    // not the left corner
    SetTransform();

    // User Draw Calls
    ENGINE.PushTransform();
#ifdef MATHEMATICAL_COORDINATESYSTEM

    ENGINE.Scale(m_WindowScale, Point2Int{ 0,m_Height });
#else
    ENGINE.Scale(m_WindowScale, Point2Int{ 0,0 });

#endif // MATHEMATICAL_COORDINATESYSTEM

    m_pGame->Draw();
    ENGINE.PopTransform();

    // Dont show more than the the scaled window size given by the user
    m_pDRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    m_TransformChanged = true;
    SetColor(RGB(0, 0, 0));
    DrawBorders(GetRenderTargetSize().width, GetRenderTargetSize().height);
    
    

    hr = m_pDRenderTarget->EndDraw();
    return hr;
}

//lines

void Engine::DrawLine(const Point2Int& firstPoint, const Point2Int& secondPoint, float lineThickness) const
{
    DrawLine(firstPoint.x, firstPoint.y, secondPoint.x, secondPoint.y, lineThickness);
   
}
void Engine::DrawLine(int firstX, int firstY, const Point2Int& secondPoint, float lineThickness) const
{
    DrawLine(Point2Int{ firstX,firstY }, secondPoint, lineThickness);
}

void Engine::DrawLine(const Point2Int& firstPoint, int secondX, int secondY, float lineThickness) const
{
    DrawLine(firstPoint.x, firstPoint.y, secondX, secondY, lineThickness);
}

void Engine::DrawVector(const Point2Int& origin, const Vector2f& vector, float lineThickness) const
{
    DrawVector(origin.x, origin.y, vector.x, vector.y, lineThickness);
}
void Engine::DrawVector(const Point2Int& origin, float vectorX, float vectorY, float lineThickness) const
{
    DrawVector(origin.x, origin.y, vectorX, vectorY, lineThickness);
}
void Engine::DrawVector(int originX, int originY, const Vector2f& vector, float lineThickness) const
{
    DrawVector(originX, originY, vector.x, vector.y, lineThickness);
}
void Engine::DrawVector(int originX, int originY, float vectorX, float vectorY, float lineThickness) const
{
    SetTransform();

    const int endX = originX + static_cast<int>(vectorX);
    const int endY = originY + static_cast<int>(vectorY);

    const int arrowLineLength{ 30 };
    const float desiredHeadAngle = float(M_PI / 12.f);
    const float mirroredVectorAngle = atan2f(vectorY, vectorX) + float(M_PI) ;

    const Point2Int arrowP2{ static_cast<int>(endX + cosf(mirroredVectorAngle - desiredHeadAngle) * arrowLineLength),
                            static_cast<int>(endY + sinf(mirroredVectorAngle - desiredHeadAngle) * arrowLineLength) };

    
    const Point2Int arrowP3{ static_cast<int>(endX + cosf(mirroredVectorAngle + desiredHeadAngle) * arrowLineLength),
                            static_cast<int>(endY + sinf(mirroredVectorAngle + desiredHeadAngle) * arrowLineLength) };

    DrawLine(originX, originY, endX, endY, lineThickness);
    DrawLine(endX, endY, arrowP2.x, arrowP2.y, lineThickness);
    DrawLine(endX, endY, arrowP3.x, arrowP3.y, lineThickness);
}

#ifdef MATHEMATICAL_COORDINATESYSTEM
void Engine::DrawLine(int firstX, int firstY, int secondX, int secondY, float lineThickness)const
{
     SetTransform();
    m_pDRenderTarget->DrawLine(
        D2D1::Point2F(static_cast<FLOAT>(firstX), static_cast<FLOAT>(m_Height - firstY)),
        D2D1::Point2F(static_cast<FLOAT>(secondX), static_cast<FLOAT>(m_Height - secondY)),
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
    SetTransform();
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
    SetTransform();
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
    SetTransform();
    m_pDRenderTarget->DrawEllipse(
        D2D1::Ellipse(
            D2D1::Point2F(static_cast<FLOAT>(ellipse.center.x), static_cast<FLOAT>(m_Height - ellipse.center.y)),
            static_cast<FLOAT>(ellipse.radiusX),
            static_cast<FLOAT>(ellipse.radiusY)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}

void Engine::DrawCircle(const CircleInt& circle, float lineThickness) const
{
    DrawEllipse(circle.center, circle.rad, circle.rad, lineThickness);
}

// Strings
void Engine::DrawString(const tstring& textToDisplay, const Font& font, int left, int bottom, int width, int height, bool showRect)const
{
    DrawString(textToDisplay, font, RectInt{ left, bottom, width, height }, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, const Font& font, Point2Int leftBottom, int width, int height, bool showRect)const
{
    DrawString(textToDisplay, font, RectInt{ leftBottom.x, leftBottom.y, width, height }, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, const Font& font, RectInt destRect, bool showRect)const
{
    SetTransform();
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
        to_wstring(textToDisplay).c_str(),
        (UINT32) textToDisplay.length(),
        font.GetFormat(),
        rect,
        m_pDColorBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);
}

void Engine::DrawString(const tstring& textToDisplay, const Font& font, int left, int bottom, int width, bool showRect)const
{
    DrawString(textToDisplay, font, Point2Int{ left, bottom }, width, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, const Font& font, Point2Int leftBottom, int width, bool showRect)const
{
    SetTransform();
    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<FLOAT>(leftBottom.x),
        static_cast<FLOAT>(m_Height - (leftBottom.y + font.GetFontSize())),
        static_cast<FLOAT>(leftBottom.x + width),
        static_cast<FLOAT>(m_Height - leftBottom.y));
    if (showRect)
    {
        m_pDRenderTarget->DrawRectangle(rect, m_pDColorBrush);
    }
    
    m_pDRenderTarget->DrawText(
        to_wstring(textToDisplay).c_str(),
        (UINT32) textToDisplay.length(),
        font.GetFormat(),
        rect,
        m_pDColorBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);
}

//Textures
void Engine::DrawTexture(const Texture& texture, int destLeft, int destBottom, const RectInt& srcRect, float opacity)const
{
    DrawTexture(texture, RectInt{ destLeft, destBottom, int(texture.GetWidth()), int(texture.GetHeight()) }, srcRect, opacity);
}
void Engine::DrawTexture(const Texture& texture, const Point2Int& destLeftBottom, const RectInt& srcRect, float opacity)const
{
    DrawTexture(texture, RectInt{ destLeftBottom.x, destLeftBottom.y, int(texture.GetWidth()), int(texture.GetHeight()) }, srcRect, opacity);
}
void Engine::DrawTexture(const Texture& texture, const RectInt& destRect, const RectInt& srcRect, float opacity)const
{
    RectInt wndwSize = ENGINE.GetWindowRect();

    D2D1_RECT_F destination = D2D1::RectF(
        static_cast<FLOAT>(destRect.left),
        static_cast<FLOAT>(wndwSize.height - (destRect.bottom + destRect.height)),
        static_cast<FLOAT>(destRect.left + destRect.width),
        static_cast<FLOAT>(wndwSize.height - destRect.bottom)
    );
    D2D1_RECT_F source{};
    if (srcRect.width <= 0 || srcRect.height <= 0)
    {
        source = D2D1::RectF(
            0.F,
            0.F,
            static_cast<FLOAT>(texture.GetWidth()),
            static_cast<FLOAT>(texture.GetHeight())
        );
    }
    else
    {
        float sliceMargin{ 0.07f };
        source = D2D1::RectF(
            static_cast<FLOAT>(srcRect.left + sliceMargin),
            static_cast<FLOAT>(srcRect.bottom + sliceMargin),
            static_cast<FLOAT>(srcRect.left + srcRect.width - sliceMargin),
            static_cast<FLOAT>(srcRect.bottom + srcRect.height - sliceMargin));
    }

    SetTransform();
    ENGINE.getRenderTarget()->DrawBitmap(
        texture.GetBitmap(),
        destination,
        opacity,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        source
    );
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
    SetTransform();
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
    SetTransform();
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
    SetTransform();
    m_pDRenderTarget->FillEllipse(
        D2D1::Ellipse(
            D2D1::Point2F(static_cast<FLOAT>(ellipse.center.x), static_cast<FLOAT>(m_Height - ellipse.center.y)),
            static_cast<FLOAT>(ellipse.radiusX),
            static_cast<FLOAT>(ellipse.radiusY)),
        m_pDColorBrush);
}
void Engine::FillCircle(const CircleInt& circle) const
{
    FillEllipse(circle.center, circle.rad, circle.rad);
}
#else

//Lines
void Engine::DrawLine(int firstX, int firstY, int secondX, int secondY, float lineThickness)const
{
    SetTransform();
    m_pDRenderTarget->DrawLine(
        D2D1::Point2F(static_cast<FLOAT>(firstX), static_cast<FLOAT>(firstY)),
        D2D1::Point2F(static_cast<FLOAT>(secondX), static_cast<FLOAT>(secondY)),
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
    SetTransform();
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
    SetTransform();
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
    SetTransform();
    m_pDRenderTarget->DrawEllipse(
        D2D1::Ellipse(
            D2D1::Point2F(static_cast<FLOAT>(ellipse.center.x), static_cast<FLOAT>(ellipse.center.y)),
            static_cast<FLOAT>(ellipse.radiusX),
            static_cast<FLOAT>(ellipse.radiusY)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}
void Engine::DrawString(const tstring& textToDisplay, const Font& font, int left, int top, int width, int height, bool showRect)const
{
    DrawString(textToDisplay, font, RectInt{ left, top, width, height }, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, const Font& font, Point2Int leftTop, int width, int height, bool showRect)const
{
    DrawString(textToDisplay, font, RectInt{ leftTop.x, leftTop.y, width, height }, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, const Font& font, RectInt destRect, bool showRect)const
{
    SetTransform();
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
        (UINT32) textToDisplay.length(),
        font.GetFormat(),
        rect,
        m_pDColorBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);
}

//Takes the size of the font as Height of the destination rectangle in order to have a logical position
void Engine::DrawString(const tstring& textToDisplay, const Font& font, int left, int top, int width, bool showRect)const
{
    DrawString(textToDisplay, font, Point2Int{ left,top }, width, showRect);
}
//Takes the size of the font as Height of the destination rectangle in order to have a logical position
void Engine::DrawString(const tstring& textToDisplay, const Font& font, Point2Int leftTop, int width, bool showRect)const
{
    SetTransform();
    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<FLOAT>(leftTop.x),
        static_cast<FLOAT>(leftTop.y),
        static_cast<FLOAT>(leftTop.x + width),
        static_cast<FLOAT>(leftTop.y + font.GetFontSize()));

    if (showRect)
    {
        m_pDRenderTarget->DrawRectangle(rect, m_pDColorBrush);
    }
    
    m_pDRenderTarget->DrawText(
        textToDisplay.c_str(),
        (UINT32) textToDisplay.length(),
        font.GetFormat(),
        rect,
        m_pDColorBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);
}

//Textures
void Engine::DrawTexture(const Texture& texture, int destLeft, int destTop, const RectInt& srcRect, float opacity)const
{
    DrawTexture(texture, RectInt{ destLeft, destTop, int(texture.GetWidth()), int(texture.GetHeight()) }, srcRect, opacity);
}
void Engine::DrawTexture(const Texture& texture, const Point2Int& destLeftTop, const RectInt& srcRect, float opacity)const
{
    DrawTexture(texture, RectInt{ destLeftTop.x, destLeftTop.y, int(texture.GetWidth()), int(texture.GetHeight()) }, srcRect, opacity);
}
void Engine::DrawTexture(const Texture& texture, const RectInt& destRect, const RectInt& srcRect, float opacity)const
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
            0.F,
            0.F,
            static_cast<FLOAT>(texture.GetWidth()),
            static_cast<FLOAT>(texture.GetHeight())
        );
    }
    else
    {
        float sliceMargin{ 0.07f };
        source = D2D1::RectF(
            static_cast<FLOAT>(srcRect.left + sliceMargin),
            static_cast<FLOAT>(srcRect.top + sliceMargin),
            static_cast<FLOAT>(srcRect.left + srcRect.width - sliceMargin),
            static_cast<FLOAT>(srcRect.top + srcRect.height - sliceMargin));
    }

    SetTransform();
    ENGINE.getRenderTarget()->DrawBitmap(
        texture.GetBitmap(),
        destination,
        opacity,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        source
    );
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
    SetTransform();
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
    SetTransform();
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
    SetTransform();
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
    return GetKeyState(virtualKeycode) < 0 and m_WindowIsActive;
}
void Engine::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
}
void Engine::SetTitle(const tstring& newTitle)
{
    m_Title.assign(newTitle);
    SetWindowText(m_hWindow, newTitle.c_str());
}
void Engine::SetWindowDimensions(int width, int height)
{
    m_Width = width;
    m_Height = height;
}
void Engine::SetWindowScale(float scale)
{
    m_WindowScale = scale;
}
void Engine::SetWindowPosition()
{
    DWORD dwAdd = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
    ::SetWindowLong(m_hWindow, GWL_STYLE, dwAdd);

    UINT dpi = GetDpiForWindow(m_hWindow);

    int windowWidth{ (GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + static_cast<int>(m_Width * m_WindowScale) + 10) };
    int windowHeight{ (GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
                        GetSystemMetrics(SM_CYCAPTION) + static_cast<int>(m_Height * m_WindowScale) + 10) };

    windowWidth = static_cast<int>(windowWidth * dpi / 96.f);
    windowHeight = static_cast<int>(windowHeight * dpi / 96.f);

    int xPos{ GetSystemMetrics(SM_CXSCREEN) / 2 - windowWidth / 2 };
    int yPos{ GetSystemMetrics(SM_CYSCREEN) / 2 - windowHeight / 2 };

    ::SetWindowPos(m_hWindow, NULL, xPos, yPos, windowWidth, windowHeight, SWP_FRAMECHANGED);

    if (m_pGame)
    {
        ShowWindow(m_hWindow, SW_SHOWNORMAL);
        UpdateWindow(m_hWindow);
    }
}
void Engine::SetFullscreen()
{
    //https://www.codeproject.com/Questions/108400/How-to-Set-Win32-Application-to-Full-Screen-C
    DWORD dwStyle = ::GetWindowLong(m_hWindow, GWL_STYLE);
    DWORD dwRemove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
    DWORD dwNewStyle = dwStyle & ~dwRemove;
    ::SetWindowLong(m_hWindow, GWL_STYLE, dwNewStyle);
    ::SetWindowPos(m_hWindow, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    HDC hDC = ::GetWindowDC(m_hWindow);
    ::SetWindowPos(m_hWindow, NULL, 0, 0,
        ::GetDeviceCaps(hDC, HORZRES),
        ::GetDeviceCaps(hDC, VERTRES),
        SWP_FRAMECHANGED);

    if (m_pGame)
    {
        ShowWindow(m_hWindow, SW_DENORMAL);
        UpdateWindow(m_hWindow);
    }
}
void Engine::SetTransform() const
{
    if (m_TransformChanged)
    {
        D2D1::Matrix3x2F combinedMatrix{D2D1::Matrix3x2F::Identity()};
        for (auto& matrix : m_VecTransformMatrices)
        {
            combinedMatrix = matrix * combinedMatrix;
        }

        m_pDRenderTarget->SetTransform(
            combinedMatrix *
            D2D1::Matrix3x2F::Scale(m_ViewPortScaling, m_ViewPortScaling) *
            D2D1::Matrix3x2F::Translation(m_ViewPortTranslationX, m_ViewPortTranslationY)

        );

        m_TransformChanged = false;
    }
}
void Engine::SetDeltaTime(float elapsedSec)
{
    m_DeltaTime = elapsedSec;
    m_TotalTime += elapsedSec;
}
void Engine::SetFrameRate(int FPS)
{
    m_MilliSecondsPerFrame = 1000.0f / FPS;
}

#ifdef MATHEMATICAL_COORDINATESYSTEM
void Engine::Translate(int xTranslation, int yTranslation)
{
    if (!m_VecTransformMatrices.empty())
    {
        auto& lastMatrix = m_VecTransformMatrices.at(m_VecTransformMatrices.size() - 1);
        lastMatrix = D2D1::Matrix3x2F::Translation(static_cast<FLOAT>(xTranslation), static_cast<FLOAT>(-yTranslation)) * lastMatrix;
    }
    else OutputDebugString(_T("Vector of matrices was empty while trying to add a Translation matrix."));

    m_TransformChanged = true;
}

void Engine::Rotate(float angle, int xPivotPoint, int yPivotPoint)
{
    if (!m_VecTransformMatrices.empty())
    {
        auto& lastMatrix = m_VecTransformMatrices.at(m_VecTransformMatrices.size() - 1);
        lastMatrix = D2D1::Matrix3x2F::Rotation(static_cast<FLOAT>(-angle), D2D1::Point2F(static_cast<FLOAT>(xPivotPoint), static_cast<FLOAT>(m_Height - yPivotPoint))) * lastMatrix;
    }
    else OutputDebugString(_T("Vector of matrices was empty while trying to add a Rotation matrix."));

    m_TransformChanged = true;
}
void Engine::Scale(float xScale, float yScale, int xPointToScaleFrom, int yPointToScaleFrom)
{
    if (!m_VecTransformMatrices.empty())
    {
        auto& lastMatrix = m_VecTransformMatrices.at(m_VecTransformMatrices.size() - 1);
        lastMatrix = D2D1::Matrix3x2F::Scale(static_cast<FLOAT>(xScale), static_cast<FLOAT>(yScale),
            D2D1::Point2F(static_cast<FLOAT>(xPointToScaleFrom), static_cast<FLOAT>(m_Height - yPointToScaleFrom)))
            * lastMatrix;
    }
    else OutputDebugString(_T("Vector of matrices was empty while trying to add a Scaling matrix."));

    m_TransformChanged = true;
}
#else
void Engine::Translate(int xTranslation, int yTranslation)
{
    if (!m_VecTransformMatrices.empty())
    {
        auto& lastMatrix = m_VecTransformMatrices.at(m_VecTransformMatrices.size() - 1);
        lastMatrix = D2D1::Matrix3x2F::Translation(static_cast<FLOAT>(xTranslation), static_cast<FLOAT>(yTranslation)) * lastMatrix;
    }
    else OutputDebugString(_T("Vector of matrices was empty while trying to add a Translation matrix."));

    m_TransformChanged = true;
}
void Engine::Rotate(float angle, int xPivotPoint, int yPivotPoint)
{
    if (!m_VecTransformMatrices.empty())
    {
        auto& lastMatrix = m_VecTransformMatrices.at(m_VecTransformMatrices.size() - 1);
        lastMatrix = D2D1::Matrix3x2F::Rotation(static_cast<FLOAT>(angle), D2D1::Point2F(static_cast<FLOAT>(xPivotPoint), static_cast<FLOAT>(yPivotPoint))) * lastMatrix;
    }
    else OutputDebugString(_T("Vector of matrices was empty while trying to add a Rotation matrix."));
}
void Engine::Scale(float xScale, float yScale, int xPointToScaleFrom, int yPointToScaleFrom)
{
    if (!m_VecTransformMatrices.empty())
    {
        auto& lastMatrix = m_VecTransformMatrices.at(m_VecTransformMatrices.size() - 1);
        lastMatrix = D2D1::Matrix3x2F::Scale(static_cast<FLOAT>(xScale), static_cast<FLOAT>(yScale), D2D1::Point2F(static_cast<FLOAT>(xPointToScaleFrom), static_cast<FLOAT>(yPointToScaleFrom))) * lastMatrix;
    }
    else OutputDebugString(_T("Vector of matrices was empty while trying to add a Scaling matrix."));

    m_TransformChanged = true;
}
#endif // MATHEMATICAL_COORDINATESYSTEM

void Engine::PushTransform()
{
    m_VecTransformMatrices.push_back(D2D1::Matrix3x2F::Identity());
}

void Engine::PopTransform()
{
    m_VecTransformMatrices.pop_back();

    m_TransformChanged = true;  
}
void Engine::Translate(const Vector2f& translation)
{
    Translate(static_cast<int>(round(translation.x)), static_cast<int>(round(translation.y)));
}
void Engine::Rotate(float angle, const Point2Int& pivotPoint)
{
    Rotate(angle, pivotPoint.x, pivotPoint.y);
}
void Engine::Scale(float scale, int xPointToScaleFrom, int yPointToScaleFrom)
{
    Scale(scale, scale, xPointToScaleFrom, yPointToScaleFrom);
}
void Engine::Scale(float xScale, float yScale, const Point2Int& PointToScaleFrom)
{
    Scale(xScale, yScale, PointToScaleFrom.x, PointToScaleFrom.y);
}
void Engine::Scale(float scale, const Point2Int& PointToScaleFrom)
{
    Scale(scale, scale, PointToScaleFrom.x, PointToScaleFrom.y);
}

void Engine::SetColor(COLORREF newColor, float opacity)
{
    //if (m_pDColorBrush) SafeRelease(&m_pDColorBrush);
    m_pDColorBrush->SetColor(D2D1::ColorF(
        GetRValue(newColor) / 255.f,
        GetGValue(newColor) / 255.f,
        GetBValue(newColor) / 255.f));

    m_pDColorBrush->SetOpacity(static_cast<FLOAT>(opacity));
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
void Engine::Paint()
{
    InvalidateRect(m_hWindow, NULL, TRUE);
    HRESULT hr = OnRender();

    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        SafeRelease(&m_pDRenderTarget);
        SafeRelease(&m_pDColorBrush);
    }
    ValidateRect(m_hWindow, NULL);
}

RectInt Engine::GetWindowRect() const
{
    return RectInt{ 0, 0, m_Width, m_Height };
}
float Engine::GetWindowScale() const
{
    return m_WindowScale;
}
HWND Engine::GetWindow() const
{
    return m_hWindow;
}
HINSTANCE Engine::GetHInstance() const
{
    return m_hInstance;
}
float Engine::GetDeltaTime() const
{
    return m_DeltaTime;
}
float Engine::GetTotalTime() const
{
    return m_TotalTime;
}
ID2D1HwndRenderTarget* Engine::getRenderTarget() const
{
    return m_pDRenderTarget;
}

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------
//TEXTURE
//---------------------

IWICImagingFactory* Texture::m_pWICFactory{ nullptr };

Texture::Texture(const tstring& filename) :
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
            CLSCTX_ALL,
            IID_PPV_ARGS(&m_pWICFactory)
        );
    }

    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICFormatConverter* pConverter = NULL;

    std::wstring filePath = to_wstring(ResourceManager::GetInstance().GetDataPath() + filename);

    if (SUCCEEDED(creationResult))
    {
        creationResult = m_pWICFactory->CreateDecoderFromFilename(
            filePath.c_str(),
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
        creationResult = ENGINE.getRenderTarget()->CreateBitmapFromWicBitmap(
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
        OutputDebugStringW((L"ERROR! File \"" + filePath + L"\" couldn't load correctly").c_str());
    }
    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pConverter);

}
Texture::~Texture()
{
    SafeRelease(&m_pDBitmap);
};
//---------------------------------------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------------------------------------------
//---------------------
//Font
//---------------------

IDWriteFactory5* Font::m_pDWriteFactory{ nullptr };

Font::Font(const tstring& fontName, bool fromFile):
    Font{fontName, 20, false, false, fromFile}
{}
Font::Font(const tstring& fontName, int size ,bool bold, bool italic, bool fromFile)
{
    if (fromFile)
    {
        std::wstring filePath = to_wstring(ResourceManager::GetInstance().GetDataPath() + fontName);
        HRESULT hr = Initialize(filePath);
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
        m_FontName = to_wstring(fontName);
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
        OutputDebugStringW((L"Something went wrong in the Font constructor using file " + fontName).c_str());
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
    const auto& createTextFormat = [&]()
        {
            m_pDWriteFactory->CreateTextFormat(
                m_FontName.c_str(),
                m_pFontCollection,
                bold ? DWRITE_FONT_WEIGHT_EXTRA_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
                italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                static_cast<FLOAT>(size),
                L"en-us",
                &m_pTextFormat);

            assert((m_pTextFormat) && _T("TexFormat was not loaded correctly"));
            m_FontSize = size;
        };

    if (not m_pTextFormat)
    {
        createTextFormat();
    }
    else if(
       m_pTextFormat->GetFontSize() != size or
       m_pTextFormat->GetFontWeight() != (bold ? DWRITE_FONT_WEIGHT_EXTRA_BOLD : DWRITE_FONT_WEIGHT_NORMAL) or
       m_pTextFormat->GetFontStyle() != (italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL))
    {
        const auto horAllign = m_pTextFormat->GetTextAlignment();
        const auto vertAllign = m_pTextFormat->GetParagraphAlignment();

        SafeRelease(&m_pTextFormat);

        createTextFormat();

        m_pTextFormat->SetTextAlignment(horAllign);
        m_pTextFormat->SetParagraphAlignment(vertAllign);
    }

}
void Font::SetHorizontalAllignment(HorAllignment allignment)
{
    switch (allignment)
    {
    case HorAllignment::Left:
        if (m_pTextFormat) m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        break;
    case HorAllignment::Center:
        if (m_pTextFormat) m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        break;
    case HorAllignment::Right:
        if (m_pTextFormat) m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        break;
    case HorAllignment::Justified:
        if (m_pTextFormat) m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
        break;
    }
}
void Font::SetVerticalAllignment(VertAllignment allignment)
{
    switch (allignment)
    {
    case Font::VertAllignment::Top:
        if (m_pTextFormat) m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        break;
    case Font::VertAllignment::Center:
        if (m_pTextFormat) m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        break;
    case Font::VertAllignment::Bottom:
        if (m_pTextFormat) m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
        break;
    }
}
IDWriteTextFormat* Font::GetFormat() const
{
    return m_pTextFormat;
}
int Font::GetFontSize() const
{
    return m_FontSize;
}
//---------------------------------------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------------------------------------------
//---------------------
// ResourceManager
//---------------------

void ResourceManager::Init(const tstring& dataPath)
{
    m_DataPath = dataPath;
}

Texture& ResourceManager::GetTexture(const tstring& file)
{
    if (not m_pMapTextures.contains(file))
    {
        m_pMapTextures[file] = std::make_unique<Texture>(file);
    }

    return *m_pMapTextures.at(file);
}

void ResourceManager::RemoveTexture(const tstring& file)
{
    if (m_pMapTextures.contains(file))
    {
        m_pMapTextures.erase(file);
    }
    else OutputDebugString((_T("Texture to remove is not present. File: ") + file).c_str());
}

void ResourceManager::RemoveAllTextures()
{
    m_pMapTextures.clear();
}

Font& ResourceManager::GetFont(const tstring& fontName, bool fromFile)
{
    if (not m_pMapFonts.contains(fontName))
    {
        m_pMapFonts[fontName] = std::make_unique<Font>(fontName, fromFile);
    }

    return *m_pMapFonts.at(fontName);
}

void ResourceManager::RemoveFont(const tstring& fontName)
{
    if (m_pMapFonts.contains(fontName))
    {
        m_pMapFonts.erase(fontName);
    }
    else OutputDebugString((_T("Font to remove is not present. File: ") + fontName).c_str());
}

void ResourceManager::RemoveAllFonts()
{
    m_pMapFonts.clear();
}
//---------------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------------
//---------------------
// Utils
//---------------------

// Following functions originate from Koen Samyn, professor Game Development at Howest

float utils::Distance(int x1, int y1, int x2, int y2)
{
    int b = x2 - x1;
    int c = y2 - y1;
    return sqrt(static_cast<float>(b * b + c * c));
}

float utils::Distance(const Point2Int& p1, const Point2Int& p2)
{
    return Distance(p1.x, p1.y, p2.x, p2.y);
}

bool utils::IsPointInRect(const Point2Int& p, const RectInt& r)
{
    return p.x >= r.left and
        p.x <= (r.left + r.width) and
#ifdef MATHEMATICAL_COORDINATESYSTEM
        p.y >= r.bottom and
        p.y <= (r.bottom + r.height);
#else
        p.y >= r.top and
        p.y <= (r.top + r.height);
#endif // MATHEMATICAL_COORDINATESYSTEM

}

bool utils::IsPointInCircle(const Point2Int& p, const CircleInt& c)
{
    int x = c.center.x - p.x;
    int y = c.center.y - p.y;
    return x * x + y * y < c.rad * c.rad;
}

bool utils::IsPointInEllipse(const Point2Int& p, const EllipseInt& e)
{
    int lhs = (p.x * p.x) * (e.radiusY * e.radiusY) + (p.y * p.y) * (e.radiusX * e.radiusX);
    int rhs = (e.radiusX * e.radiusX) * (e.radiusY * e.radiusY);
    return lhs <= rhs;
}

bool utils::IsOverlapping(const Point2Int& a, const Point2Int& b, const CircleInt& c)
{
    return DistPointLineSegment(c.center, a, b) <= c.rad;
}

bool utils::IsOverlapping(const Point2Int& a, const Point2Int& b, const EllipseInt& e)
{
    return IsPointInEllipse(ClosestPointOnLine(e.center, a, b), e);
}

bool utils::IsOverlapping(const Point2Int& a, const Point2Int& b, const RectInt& r)
{
    std::pair<Point2Int, Point2Int> p{};
    return utils::IntersectRectLine(r, a, b, p);
}

bool utils::IsOverlapping(const RectInt& r1, const RectInt& r2)
{

#ifdef MATHEMATICAL_COORDINATESYSTEM
    if ((r1.left + r1.width) < r2.left || (r2.left + r2.width) < r1.left ||
        r1.bottom > (r2.bottom + r2.height) || r2.bottom > (r1.bottom + r1.height))
    {
        return false;
    }
#else
    if ((r1.left + r1.width) < r2.left || (r2.left + r2.width) < r1.left ||
        (r1.top + r1.height) < r2.top || (r2.top + r2.height) < r1.top)
    {
        return false;
    }
#endif // MATHEMATICAL_COORDINATESYSTEM    

    return true;
}

bool utils::IsOverlapping(const RectInt& r, const CircleInt& c)
{
    if (IsPointInRect(c.center, r)) return true;

    int right = r.left + r.width;

#ifdef MATHEMATICAL_COORDINATESYSTEM
    int top = r.bottom + r.height;
    if (DistPointLineSegment(c.center, Point2Int{ r.left, r.bottom }, Point2Int{ r.left, top }) <= c.rad) return true;
    
    if (DistPointLineSegment(c.center, Point2Int{ r.left, r.bottom }, Point2Int{ right, r.bottom }) <= c.rad) return true;
    
    if (DistPointLineSegment(c.center, Point2Int{ r.left, top }, Point2Int{ right, top }) <= c.rad) return true;
    
    if (DistPointLineSegment(c.center, Point2Int{ right, top }, Point2Int{ right, r.bottom }) <= c.rad) return true;
#else
    int bottom = r.top + r.height;
    if (DistPointLineSegment(c.center, Point2Int{ r.left, r.top }, Point2Int{ r.left, bottom }) <= c.rad) return true;
    
    if (DistPointLineSegment(c.center, Point2Int{ r.left, r.top }, Point2Int{ right, r.top }) <= c.rad) return true;
    
    if (DistPointLineSegment(c.center, Point2Int{ r.left, bottom }, Point2Int{ right, bottom }) <= c.rad) return true;
    
    if (DistPointLineSegment(c.center, Point2Int{ right, bottom }, Point2Int{ right, r.top }) <= c.rad) return true;
    
#endif // MATHEMATICAL_COORDINATESYSTEM

    return false;
}

bool utils::IsOverlapping(const CircleInt& c1, const CircleInt& c2)
{
    return (c2.center - c1.center).SquaredLength() < (c1.rad + c2.rad) * (c1.rad + c2.rad);
}


Point2Int utils::ClosestPointOnLine(const Point2Int& point, const Point2Int& linePointA, const Point2Int& linePointB)
{
    Vector2f aToB{ linePointA, linePointB };
    Vector2f aToPoint{ linePointA, point };
    Vector2f abNorm{ aToB.Normalized() };
    float pointProjectionOnLine{ Vector2f::Dot(abNorm, aToPoint) };

    // If pointProjectionOnLine is negative, then the closest point is A
    if (pointProjectionOnLine < 0) return linePointA;

    // If pointProjectionOnLine is > than dist(linePointA,linePointB) then the closest point is B
    float distAB{ aToB.Length() };
    if (pointProjectionOnLine > distAB) return linePointB;

    // Closest point is between A and B, calc intersection point
    Point2Int intersection{ linePointA + pointProjectionOnLine * abNorm };
    return intersection;
}

float utils::DistPointLineSegment(const Point2Int& point, const Point2Int& linePointA, const Point2Int& linePointB)
{
    return (point - ClosestPointOnLine(point, linePointA, linePointB)).Length();
}

bool utils::IsPointOnLineSegment(const Point2Int& point, const Point2Int& linePointA, const Point2Int& linePointB)
{
    Vector2f aToPoint{ linePointA, point };
    Vector2f bToPoint{ linePointB, point };

    // If not on same line, return false
    if (abs(Vector2f::Cross(aToPoint, bToPoint)) > 0.0001f) return false;

    // Both vectors must point in opposite directions if p is between a and b
    if (Vector2f::Dot(aToPoint, bToPoint) > 0) return false;

    return true;
}

bool utils::IntersectLineSegments(const Point2Int& p1, const Point2Int& p2, const Point2Int& q1, const Point2Int& q2, float& outLambda1, float& outLambda2)
{
    bool intersecting{ false };

    Vector2f firstLine{ p1, p2 };
    Vector2f secondLine{ q1, q2 };

    float denom = Vector2f::Cross(firstLine,secondLine);

    if (std::abs(denom) > 0.0001f)
    {
        intersecting = true;

        Vector2f p1q1{ p1, q1 };

        float num1 = Vector2f::Cross( p1q1, secondLine);
        float num2 = Vector2f::Cross( p1q1, firstLine);
        
        outLambda1 = num1 / denom;
        outLambda2 = num2 / denom;
    }
    else // are parallel
    {
        Vector2f p1q1{ p1, q1 };

        if (std::abs(Vector2f::Cross(p1q1,secondLine)) > 0.0001f) return false;

        outLambda1 = 0;
        outLambda2 = 0;
        if (utils::IsPointOnLineSegment(p1, q1, q2) ||
            utils::IsPointOnLineSegment(p2, q1, q2))
        {
            intersecting = true;
        }
    }
    return intersecting;
}

bool utils::IntersectRectLine(const RectInt& r, const Point2Int& p1, const Point2Int& p2, std::pair<Point2Int, Point2Int>& intersections)
{
    float xDenom{ static_cast<float>(p2.x - p1.x) };
    float x1{ (r.left - p1.x) / xDenom};
    float x2{ (r.left + r.width - p1.x) / xDenom };

    float yDenom{ static_cast<float>(p2.y - p1.y) };
#ifdef MATHEMATICAL_COORDINATESYSTEM
    float y1{ (r.bottom - p1.y  ) / yDenom };
    float y2{ (r.bottom + r.height - p1.y ) / yDenom };
#else  
    float y1{ (r.top - p1.y) / yDenom };
    float y2{ (r.top + r.height- p1.y) / yDenom };
#endif // !MATHEMATICAL_COORDINATESYSTEM
    

    float tMin{ std::max<float>(std::min<float>(x1,x2), std::min<float>(y1,y2)) };
    float tMax{ std::min<float>(std::max<float>(x1,x2), std::max<float>(y1,y2)) };

    if (tMin > tMax) return false;

    Vector2f lineDirection{ p1, p2 };
    intersections.first = p1 + lineDirection * tMin;
    intersections.second = p1 + lineDirection * tMax;

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------