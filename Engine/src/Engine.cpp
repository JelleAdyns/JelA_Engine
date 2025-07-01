
#include "Engine.h"
#include <chrono>
#include <thread>
#include <algorithm>

namespace jela
{
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ENGINE.HandleMessages(hWnd, message, wParam, lParam);
    }
    
    Engine::Engine() :
        m_hWindow{ NULL },
        m_hInstance{ NULL },
        m_OriginalStyle{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX },
        m_pDFactory{ NULL },
        m_pDRenderTarget{NULL},
        m_pDColorBrush{NULL},
        m_DColorBackGround{ D2D1::ColorF::Black},
        m_pGame{ nullptr },
        m_Title{ _T("Standard Game")},
        m_GameWidth{500},
        m_GameHeight{500},
        m_MilliSecondsPerFrame{1.f/60.f},
        m_IsFullscreen{false},
        m_KeyIsDown{false}
    {
        
    }

    void Engine::Shutdown()
    {
        m_pGame->Cleanup();
        m_pGame = nullptr;

        AudioLocator::RegisterAudioService(nullptr);

        m_pResourceManager = nullptr;
 
        SafeRelease(&m_pDBitmap);
        SafeRelease(&m_pDBitmapRenderTarget);
        SafeRelease(&m_pDColorBrush);
        SafeRelease(&m_pDRenderTarget);
        SafeRelease(&m_pDFactory);

        CoUninitialize();
    }

    LRESULT Engine::HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        LRESULT result = 0;
        
        bool wasHandled = false;
        if (m_pGame)
        {

            int xCoordinate = static_cast<int>(
                std::round(
                    (GET_X_LPARAM(lParam) - m_ViewPortTranslationX) / 
                    (m_WindowWidth - m_ViewPortTranslationX * 2) * 
                    m_GameWidth
                ));
            int yCoordinate = static_cast<int>(
                std::round(
                    (GET_Y_LPARAM(lParam) - m_ViewPortTranslationY) /
                    (m_WindowHeight - m_ViewPortTranslationY * 2) *
                    m_GameHeight
                ));
#ifdef MATHEMATICAL_COORDINATESYSTEM
            yCoordinate = m_GameHeight - yCoordinate;
#endif // MATHEMATICAL_COORDINATESYSTEM

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
    
                    m_WindowWidth = GetRenderTargetSize().width;
                    m_WindowHeight = GetRenderTargetSize().height;
    
                    float scaleX{ m_WindowWidth / (m_GameWidth * m_WindowScale) };
                    float scaleY{ m_WindowHeight / (m_GameHeight * m_WindowScale) };
                    float minScale{ std::min<float>(scaleX,scaleY) };

                    m_ViewPortTranslationX = (m_WindowWidth - (m_GameWidth * m_WindowScale) * minScale) / 2.f;
                    m_ViewPortTranslationY = (m_WindowHeight - (m_GameHeight * m_WindowScale) * minScale) / 2.f;

                    Paint();
                }
                
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
            case WM_DISPLAYCHANGE:
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
    
                m_IsKeyboardActive = true;
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_KEYDOWN:
            {
                if ((lParam & (1 << 30)) == 0)
                {
                    m_pGame->KeyDownThisFrame(static_cast<int>(wParam));
                }
                m_pGame->KeyDown(static_cast<int>(wParam));
    
                m_IsKeyboardActive = true;
            }
            result = 0;
            wasHandled = true;
            break;
    
            case WM_LBUTTONDOWN:
                m_pGame->MouseDown(true, xCoordinate, yCoordinate);
                result = 0;
                wasHandled = true;
                break;
            case WM_LBUTTONUP:
                m_pGame->MouseUp(true, xCoordinate, yCoordinate);
                result = 0;
                wasHandled = true;
                break;
            case WM_RBUTTONDOWN:
                m_pGame->MouseDown(false, xCoordinate, yCoordinate);
                result = 0;
                wasHandled = true;
                break;
            case WM_RBUTTONUP:
                m_pGame->MouseUp(false, xCoordinate, yCoordinate);
                result = 0;
                wasHandled = true;
                break;
            case WM_MOUSEMOVE:
                m_pGame->MouseMove(xCoordinate, yCoordinate, static_cast<int>(wParam));
                result = 0;
                wasHandled = true;
                break;
            case WM_MOUSEWHEEL:
                m_pGame->MouseWheelTurn(xCoordinate, yCoordinate, GET_WHEEL_DELTA_WPARAM(wParam), LOWORD(wParam));
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
            
        return result;
    }

    int Engine::Run(std::unique_ptr<BaseGame>&& game)
    {
        int result = 0;
        bool ableToRun = true;

        m_pGame = std::move(game);
        m_pGame->Initialize();

        SetWindowPosition();

        MSG msg;
        
        if(ableToRun)
        {
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
    
                    SetDeltaTime(elapsedSec);
    
                    m_T1 = t2;
    
                    if (IsAnyButtonPressed()) m_IsKeyboardActive = false;
    
                    for (auto& controller : m_pVecControllers)
                    {
                        controller->ProcessControllerInput();
                    }
    
                    if (not m_IsKeyboardActive)
                    {
                        m_pGame->HandleControllerInput();
                    }
    
                    m_pGame->Tick();
                    Paint();
    
                    const auto sleepTime = t2 + std::chrono::milliseconds(static_cast<int>(m_MilliSecondsPerFrame)) - std::chrono::high_resolution_clock::now();
                    std::this_thread::sleep_for(sleepTime);
    
                }
            }
    
        }
    
        return (int)msg.wParam;
    }
    bool Engine::Init(HINSTANCE hInstance, const tstring& resourcePath, int width, int height, const COLORREF& bgColor, const tstring& wndwName)
    {
        // Use HeapSetInformation to specify that the process should terminate if the heap manager detects an error in any heap used by the process.
       // The return value is ignored, because we want to continue running in the unlikely event that HeapSetInformation fails.
        HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
    
        if (SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED))/* && SUCCEEDED(MFStartup(MF_VERSION))*/)
        {
            SetInstance(hInstance);

            m_GameWidth = width;
            m_GameHeight = height;
            m_WindowWidth = width;
            m_WindowHeight = height;

            m_Title = wndwName;
            SetBackGroundColor(bgColor);
            SetFrameRate(60);

            m_pResourceManager = std::make_unique<ResourceManager>(resourcePath);
            m_pResourceManager->Start();

            HRESULT hr{ S_OK };
            hr = MakeWindow();

            if (SUCCEEDED(hr))
            {
                hr = CreateRenderTargets(); // ALWAYS CREATE RENDERTARGET BEFORE CALLING CONSTRUCTOR OF pGAME.
                // TEXTURES ARE CREATED IN THE CONSTRUCTOR AND THEY NEED THE RENDERTARGET. 

                srand(static_cast<unsigned int>(time(nullptr)));

                return SUCCEEDED(hr);
            }
        }
        return false;
    }

    void Engine::Quit()
    {
        PostMessage(GetWindow(), WM_DESTROY, NULL, NULL);
    }
    HRESULT Engine::MakeWindow()
    {
        HRESULT hr = S_OK;
    
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_pDFactory);
    
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

            //https://stackoverflow.com/questions/77638632/how-to-add-an-icon-to-a-win32api-window-in-a-cmake-project
            HICON hIcon = static_cast<HICON>(LoadImage(
                m_hInstance,
                (m_pResourceManager->GetDataPath() + _T("WindowIcon.ico")).c_str(),
                IMAGE_ICON, 
                GetSystemMetrics(SM_CXICON),
                GetSystemMetrics(SM_CYICON),
                LR_LOADFROMFILE | LR_DEFAULTSIZE));

            wcex.hIcon = hIcon;
            wcex.hIconSm = hIcon;

            RegisterClassEx(&wcex);
    
            m_hWindow = CreateWindow(m_Title.c_str(), m_Title.c_str(), WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, NULL, CW_USEDEFAULT, NULL, nullptr, nullptr, m_hInstance, nullptr);
    
            //if (m_hWindow) SetWindowPosition();
        }
    
        return hr;
       
    }
    HRESULT Engine::CreateRenderTargets()
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
    
            hr = m_pDRenderTarget->CreateCompatibleRenderTarget(
                D2D1::SizeF(static_cast<FLOAT>(m_GameWidth), static_cast<FLOAT>(m_GameHeight)),
                D2D1::SizeU(m_GameWidth, m_GameHeight),
                &m_pDBitmapRenderTarget);

            if (!m_pDColorBrush)
            {
                m_pDBitmapRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f), &m_pDColorBrush);
            }
        }
    
        return hr;
    }
    HRESULT Engine::OnRender()
    {
        HRESULT hr = S_OK;
    
        hr = CreateRenderTargets();

        //-------------------------------------------------------
        // DRAW TO BITMAP
        m_pDBitmapRenderTarget->BeginDraw();
    
        // Clear background
        m_pDBitmapRenderTarget->Clear(m_DColorBackGround);
        SafeRelease(&m_pDBitmap);

        m_pGame->Draw();

        hr = m_pDBitmapRenderTarget->EndDraw();
        //-------------------------------------------------------


        //-------------------------------------------------------
        //DRAW BITMAP TO SCREEN
        m_pDRenderTarget->BeginDraw();

        // Clear background
        m_pDRenderTarget->Clear(D2D1::ColorF(0.F, 0.F, 0.F, 1.F));
       
        m_pDBitmapRenderTarget->GetBitmap(&m_pDBitmap);

        // When the window changes in size,
        // the user draw calls should always appear in the middle of the screen,
        // not the left corner
        if (m_pDBitmap)
        {
            m_pDRenderTarget->DrawBitmap(
                m_pDBitmap,
                D2D1::RectF(
                    static_cast<FLOAT>(m_ViewPortTranslationX),
                    static_cast<FLOAT>(m_ViewPortTranslationY),
                    static_cast<FLOAT>(m_WindowWidth - m_ViewPortTranslationX),
                    static_cast<FLOAT>(m_WindowHeight - m_ViewPortTranslationY)
                ),
                static_cast<FLOAT>(1.f),
                D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
            );
        }
        
        hr = m_pDRenderTarget->EndDraw();
        //-------------------------------------------------------
        
        return hr;
    }
    
    //lines
    
    void Engine::DrawLine(const Point2Int& firstPoint, const Point2Int& secondPoint, float lineThickness) const
    {
        DrawLine(firstPoint.x, firstPoint.y, secondPoint.x, secondPoint.y, lineThickness);
       
    }
    void Engine::DrawLine(int firstX, int firstY, const Point2Int& secondPoint, float lineThickness) const
    {
        DrawLine(firstX, firstY, secondPoint.x, secondPoint.y, lineThickness);
    }
    
    void Engine::DrawLine(const Point2Int& firstPoint, int secondX, int secondY, float lineThickness) const
    {
        DrawLine(firstPoint.x, firstPoint.y, secondX, secondY, lineThickness);
    }
    
    void Engine::DrawVector(const Point2Int& origin, const Vector2f& vector, int headLineLength, float lineThickness) const
    {
        DrawVector(origin.x, origin.y, vector.x, vector.y, headLineLength, lineThickness);
    }
    void Engine::DrawVector(const Point2Int& origin, float vectorX, float vectorY, int headLineLength, float lineThickness) const
    {
        DrawVector(origin.x, origin.y, vectorX, vectorY, headLineLength, lineThickness);
    }
    void Engine::DrawVector(int originX, int originY, const Vector2f& vector, int headLineLength, float lineThickness) const
    {
        DrawVector(originX, originY, vector.x, vector.y, headLineLength, lineThickness);
    }
    void Engine::DrawVector(int originX, int originY, float vectorX, float vectorY, int headLineLength, float lineThickness) const
    {
        SetTransform();
    
        const int endX = originX + static_cast<int>(vectorX);
        const int endY = originY + static_cast<int>(vectorY);
    
        const float desiredHeadAngle = float(M_PI / 12.f);
        const float mirroredVectorAngle = atan2f(vectorY, vectorX) + float(M_PI) ;
    
        const Point2Int arrowP2{ static_cast<int>(endX + cosf(mirroredVectorAngle - desiredHeadAngle) * headLineLength),
                                static_cast<int>(endY + sinf(mirroredVectorAngle - desiredHeadAngle) * headLineLength) };
    
        
        const Point2Int arrowP3{ static_cast<int>(endX + cosf(mirroredVectorAngle + desiredHeadAngle) * headLineLength),
                                static_cast<int>(endY + sinf(mirroredVectorAngle + desiredHeadAngle) * headLineLength) };
    
        DrawLine(originX, originY, endX, endY, lineThickness);
        DrawLine(endX, endY, arrowP2.x, arrowP2.y, lineThickness);
        DrawLine(endX, endY, arrowP3.x, arrowP3.y, lineThickness);
    }

    #ifdef MATHEMATICAL_COORDINATESYSTEM
    void Engine::DrawLine(int firstX, int firstY, int secondX, int secondY, float lineThickness)const
    {
         SetTransform();
        m_pDBitmapRenderTarget->DrawLine(
            D2D1::Point2F(static_cast<FLOAT>(firstX), static_cast<FLOAT>(m_GameHeight - firstY)),
            D2D1::Point2F(static_cast<FLOAT>(secondX), static_cast<FLOAT>(m_GameHeight - secondY)),
            m_pDColorBrush,
            static_cast<FLOAT>(lineThickness)
        );
    }
    
    
    //Rectangles

    void Engine::DrawRectangle(const Point2Int& leftBottom, int width, int height, float lineThickness)const
    {
        DrawRectangle( leftBottom.x, leftBottom.y, width, height, lineThickness);
    }
    void Engine::DrawRectangle(const RectInt& rect, float lineThickness)const
    {
        DrawRectangle( rect.left, rect.bottom ,rect.width, rect.height, lineThickness);
    } 
    void Engine::DrawRectangle(int left, int bottom, int width, int height, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawRectangle(
            D2D1::RectF(
                static_cast<FLOAT>(left),
                static_cast<FLOAT>(m_GameHeight - (bottom + height)),
                static_cast<FLOAT>(left + width),
                static_cast<FLOAT>(m_GameHeight - bottom)),
            m_pDColorBrush,
            static_cast<FLOAT>(lineThickness));
       
    }
    
    //RoundedRects
    void Engine::DrawRoundedRect(const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY, float lineThickness)const
    {
        DrawRoundedRect(leftBottom.x, leftBottom.y, width, height, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawRoundedRect(const RectInt& rect, float radiusX, float radiusY, float lineThickness)const
    {
        DrawRoundedRect(rect.left, rect.bottom, rect.width, rect.height, radiusX, radiusY, lineThickness);
    }   
    void Engine::DrawRoundedRect(int left, int bottom, int width, int height, float radiusX, float radiusY, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawRoundedRectangle(
            D2D1::RoundedRect(
                D2D1::RectF(
                    static_cast<FLOAT>(left),
                    static_cast<FLOAT>(m_GameHeight - (bottom + height)),
                    static_cast<FLOAT>(left + width),
                    static_cast<FLOAT>(m_GameHeight - bottom)),
                static_cast<FLOAT>(radiusX),
                static_cast<FLOAT>(radiusY)),
            m_pDColorBrush,
            static_cast<FLOAT>(lineThickness));
        
    }


    
    // Strings
    void Engine::DrawString(const tstring& textToDisplay, const Point2Int& leftBottom, int width, int height, bool showRect)const
    {
        DrawString(textToDisplay, leftBottom.x, leftBottom.y, width, height, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, const RectInt& destRect, bool showRect)const
    {
        DrawString(textToDisplay, destRect.left, destRect.bottom, destRect.width, destRect.height, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, int left, int bottom, int width, int height, bool showRect)const
    {
        SetTransform();
        D2D1_RECT_F rect = D2D1::RectF(
            static_cast<FLOAT>(left),
            static_cast<FLOAT>(m_GameHeight - (bottom + height)),
            static_cast<FLOAT>(left + width),
            static_cast<FLOAT>(m_GameHeight - bottom));
    
        if (showRect)
        {
            m_pDBitmapRenderTarget->DrawRectangle(rect, m_pDColorBrush);
        }
    
       m_pDBitmapRenderTarget->DrawText(
           to_wstring(textToDisplay).c_str(),
           (UINT32) textToDisplay.length(),
           m_pResourceManager->GetCurrentTextFormat()->GetTextFormat(),
           rect,
           m_pDColorBrush,
           D2D1_DRAW_TEXT_OPTIONS_NONE,
           DWRITE_MEASURING_MODE_NATURAL);
        
    }

    void Engine::DrawString(const tstring& textToDisplay, const Point2Int& leftBottom, int width, bool showRect)const
    {
       DrawString(textToDisplay, leftBottom.x, leftBottom.y, width, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, int left, int bottom, int width, bool showRect)const
    {
        SetTransform();
        D2D1_RECT_F rect = D2D1::RectF(
            static_cast<FLOAT>(left),
            static_cast<FLOAT>(m_GameHeight - (bottom + m_pResourceManager->GetCurrentTextFormat()->GetFontSize())),
            static_cast<FLOAT>(left + width),
            static_cast<FLOAT>(m_GameHeight - bottom));
        if (showRect)
        {
            m_pDBitmapRenderTarget->DrawRectangle(rect, m_pDColorBrush);
        }
        
        m_pDBitmapRenderTarget->DrawText(
            to_wstring(textToDisplay).c_str(),
            (UINT32) textToDisplay.length(),
            m_pResourceManager->GetCurrentTextFormat()->GetTextFormat(),
            rect,
            m_pDColorBrush,
            D2D1_DRAW_TEXT_OPTIONS_NONE,
            DWRITE_MEASURING_MODE_NATURAL);
    }
    
    //Textures
    void Engine::DrawTexture(const Texture* const texture, int destLeft, int destBottom, const RectInt& srcRect, float opacity)const
    {
        DrawTexture(texture, RectInt{ destLeft, destBottom, texture ? int(texture->GetWidth()) : 50, texture ? int(texture->GetHeight()) : 50 }, srcRect, opacity);
    }
    void Engine::DrawTexture(const Texture* const texture, const Point2Int& destLeftBottom, const RectInt& srcRect, float opacity)const
    {
        DrawTexture(texture, RectInt{ destLeftBottom.x, destLeftBottom.y, texture ? int(texture->GetWidth()) : 50, texture ? int(texture->GetHeight()) : 50 }, srcRect, opacity);
    }
    void Engine::DrawTexture(const Texture* const texture, const RectInt& destRect, const RectInt& srcRect, float opacity)const
    {
        RectInt wndwSize = GetWindowRect();

        D2D1_RECT_F destination = D2D1::RectF(
            static_cast<FLOAT>(destRect.left),
            static_cast<FLOAT>(wndwSize.height - (destRect.bottom + destRect.height)),
            static_cast<FLOAT>(destRect.left + destRect.width),
            static_cast<FLOAT>(wndwSize.height - destRect.bottom)
        );
        D2D1_RECT_F source{};
        if ((srcRect.width <= 0 || srcRect.height <= 0) && texture)
        {
            source = D2D1::RectF(
                0.F,
                0.F,
                static_cast<FLOAT>(texture->GetWidth()),
                static_cast<FLOAT>(texture->GetHeight())
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
        if (texture)
        {
            m_pDBitmapRenderTarget->DrawBitmap(
                texture->GetBitmap(),
                destination,
                opacity,
                D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
                source
            );
        }
        else
        {
            FillRectangle(destRect);
            OutputDebugString(_T("ERROR! Texture was nullptr in DrawTexture!\n"));
        }
    }

    //Ellipses  
    void Engine::DrawEllipse(const Point2Int& center, float radiusX, float radiusY, float lineThickness)const
    {
        DrawEllipse(center.x, center.y, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawEllipse(const EllipseInt& ellipse, float lineThickness)const
    {
        DrawEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY, lineThickness);
    }
    void Engine::DrawEllipse(int centerX, int centerY, float radiusX, float radiusY, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(static_cast<FLOAT>(centerX), static_cast<FLOAT>(m_GameHeight - centerY)),
                static_cast<FLOAT>(radiusX),
                static_cast<FLOAT>(radiusY)),
            m_pDColorBrush,
            static_cast<FLOAT>(lineThickness));

    }
    void Engine::DrawCircle(const CircleInt& circle, float lineThickness) const
    {
        DrawEllipse(circle.center, circle.rad, circle.rad, lineThickness);
    }

    //-----------------
    //Fill
    //----------------

    //Rectangles
    void Engine::FillRectangle(const Point2Int& leftBottom, int width, int height)const
    {
        FillRectangle(leftBottom.x, leftBottom.y, width, height);
    }
    void Engine::FillRectangle(const RectInt& rect)const
    {
        FillRectangle(rect.left, rect.bottom, rect.width, rect.height);
    }
    void Engine::FillRectangle(int left, int bottom, int width, int height)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillRectangle(
            D2D1::RectF(
                static_cast<FLOAT>(left),
                static_cast<FLOAT>(m_GameHeight - (bottom + height)),
                static_cast<FLOAT>(left + width),
                static_cast<FLOAT>(m_GameHeight - bottom)),
            m_pDColorBrush);

    }

    //RoundedRects
    void Engine::FillRoundedRect(int left, int bottom, int width, int height, float radiusX, float radiusY)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillRoundedRectangle(
            D2D1::RoundedRect(
                D2D1::RectF(
                    static_cast<FLOAT>(left),
                    static_cast<FLOAT>(m_GameHeight - (bottom + height)),
                    static_cast<FLOAT>(left + width),
                    static_cast<FLOAT>(m_GameHeight - bottom)),
                static_cast<FLOAT>(radiusX),
                static_cast<FLOAT>(radiusY)),
            m_pDColorBrush);
    }
    void Engine::FillRoundedRect(const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY)const
    {
        FillRoundedRect(leftBottom.x, leftBottom.y, width, height, radiusX, radiusY);
    }
    void Engine::FillRoundedRect(const RectInt& rect, float radiusX, float radiusY)const
    {
        FillRoundedRect(rect.left, rect.bottom, rect.width, rect.height, radiusX, radiusY);
    }

    //Polygon
    void Engine::CreatePolygon(ID2D1PathGeometry* pGeo, const std::vector<Point2Int>& points, bool closeSegment) const
    {

        ID2D1GeometrySink* pSink;
        HRESULT hr = (pGeo)->Open(&pSink);

        if (SUCCEEDED(hr))
        {
            std::vector<D2D1_POINT_2F> D2points(points.size());

            for (size_t i = 0; i < points.size(); i++)
            {
                D2points[i] = D2D1::Point2F(static_cast<FLOAT>(points[i].x), static_cast<FLOAT>(m_GameHeight - points[i].y));
            }

            pSink->BeginFigure(D2points[0], D2D1_FIGURE_BEGIN_FILLED);
            for (size_t i = 1; i < points.size(); i++)
            {
                pSink->AddLine(D2points[i]);
            }
            pSink->EndFigure(closeSegment ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
            pSink->Close();
        }
        SafeRelease(&pSink);

    }

    //Arc
    void Engine::CreateArc(ID2D1PathGeometry* pGeo, const Point2Int& center, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment) const
    {

        ID2D1GeometrySink* pSink;

        HRESULT hr = (pGeo)->Open(&pSink);

        if (SUCCEEDED(hr))
        {
            auto startRad = (startAngle + (angle < 0.f ? angle : 0)) * M_PI / 180;
            auto endRad = (startAngle + (angle > 0.f ? angle : 0)) * M_PI / 180;

            auto beginPoint = D2D1::Point2F(
                static_cast<FLOAT>(center.x + radiusX * std::cos(startRad)),
                static_cast<FLOAT>(m_GameHeight - (center.y + radiusY * std::sin(startRad)))
            );
            auto endPoint = D2D1::Point2F(
                static_cast<FLOAT>(center.x + radiusX * std::cos(endRad)),
                static_cast<FLOAT>(m_GameHeight - (center.y + radiusY * std::sin(endRad)))
            );

            D2D1_ARC_SEGMENT arcSegment{
                endPoint,
                D2D1::SizeF(static_cast<FLOAT>(radiusX), static_cast<FLOAT>(radiusY)),
                0,
                D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
                std::abs(angle) < 180.f ? D2D1_ARC_SIZE_SMALL : D2D1_ARC_SIZE_LARGE
            };



            pSink->BeginFigure(beginPoint, D2D1_FIGURE_BEGIN_FILLED);
            pSink->AddArc(arcSegment);
            if (closeSegment) pSink->AddLine(D2D1::Point2F(static_cast<FLOAT>(center.x), static_cast<FLOAT>(m_GameHeight - center.y)));
            pSink->EndFigure(closeSegment ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
            pSink->Close();
        }

        SafeRelease(&pSink);
    }

    //Ellipses
    void Engine::FillEllipse(int centerX, int centerY, float radiusX, float radiusY)const
    {
        FillEllipse(EllipseInt{ centerX, centerY, radiusX, radiusY });
    }
    void Engine::FillEllipse(const Point2Int& center, float radiusX, float radiusY)const
    {
        FillEllipse(EllipseInt{ center, radiusX, radiusY });
    }
    void Engine::FillEllipse(const EllipseInt& ellipse)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(static_cast<FLOAT>(ellipse.center.x), static_cast<FLOAT>(m_GameHeight - ellipse.center.y)),
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
    m_pDBitmapRenderTarget->DrawLine(
        D2D1::Point2F(static_cast<FLOAT>(firstX), static_cast<FLOAT>(firstY)),
        D2D1::Point2F(static_cast<FLOAT>(secondX), static_cast<FLOAT>(secondY)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness)
    );
}


//Rectangles

void Engine::DrawRectangle(const Point2Int& leftTop, int width, int height, float lineThickness)const
{
    DrawRectangle(leftTop.x, leftTop.y, width, height, lineThickness);
}
void Engine::DrawRectangle(const RectInt& rect, float lineThickness)const
{
    DrawRectangle(rect.left, rect.top, rect.width, rect.height, lineThickness);
}
void Engine::DrawRectangle(int left, int top, int width, int height, float lineThickness)const
{
    SetTransform();
    m_pDBitmapRenderTarget->DrawRectangle(
        D2D1::RectF(
            static_cast<FLOAT>(left),
            static_cast<FLOAT>(top),
            static_cast<FLOAT>(left + width),
            static_cast<FLOAT>(top + height)),
        m_pDColorBrush,
        static_cast<FLOAT>(lineThickness));
}

//RoundedRects
void Engine::DrawRoundedRect(const Point2Int& leftTop, int width, int height, float radiusX, float radiusY, float lineThickness)const
{
    DrawRoundedRect(leftTop.x, leftTop.y, width, height, radiusX, radiusY, lineThickness);
}
void Engine::DrawRoundedRect(const RectInt& rect, float radiusX, float radiusY, float lineThickness)const
{
    DrawRoundedRect(rect.left, rect.top, rect.width, rect.height, radiusX, radiusY, lineThickness);
}
void Engine::DrawRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY, float lineThickness)const
{
    SetTransform();
    m_pDBitmapRenderTarget->DrawRoundedRectangle(
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

//String  
void Engine::DrawString(const tstring& textToDisplay, const Point2Int& leftTop, int width, int height, bool showRect)const
{
    DrawString(textToDisplay, leftTop.x, leftTop.y, width, height, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, const RectInt& destRect, bool showRect)const
{
    DrawString(textToDisplay, destRect.left, destRect.top, destRect.width, destRect.height, showRect);
}
void Engine::DrawString(const tstring& textToDisplay, int left, int top, int width, int height, bool showRect)const
{
    SetTransform();
    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<FLOAT>(left),
        static_cast<FLOAT>(top),
        static_cast<FLOAT>(left + width),
        static_cast<FLOAT>(top + height));

    if (showRect)
    {
        m_pDBitmapRenderTarget->DrawRectangle(rect, m_pDColorBrush);
    }

    m_pDBitmapRenderTarget->DrawText(
        textToDisplay.c_str(),
        (UINT32)textToDisplay.length(),
        m_pResourceManager->GetCurrentTextFormat()->GetTextFormat(),
        rect,
        m_pDColorBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);

}

//Takes the size of the font as Height of the destination rectangle in order to have a logical position
void Engine::DrawString(const tstring& textToDisplay, const Point2Int& leftTop, int width, bool showRect)const
{
    DrawString(textToDisplay, leftTop.x, leftTop.y, width, showRect);
}
//Takes the size of the font as Height of the destination rectangle in order to have a logical position
void Engine::DrawString(const tstring& textToDisplay, int left, int top, int width, bool showRect)const
{
    SetTransform();
    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<FLOAT>(left),
        static_cast<FLOAT>(top),
        static_cast<FLOAT>(left + width),
        static_cast<FLOAT>(top + m_pResourceManager->GetCurrentTextFormat()->GetFontSize()));

    if (showRect)
    {
        m_pDBitmapRenderTarget->DrawRectangle(rect, m_pDColorBrush);
    }

    m_pDBitmapRenderTarget->DrawText(
        textToDisplay.c_str(),
        (UINT32)textToDisplay.length(),
        m_pResourceManager->GetCurrentTextFormat()->GetTextFormat(),
        rect,
        m_pDColorBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);
}



//Textures
void Engine::DrawTexture(const Texture* const texture, int destLeft, int destTop, const RectInt& srcRect, float opacity)const
{
    DrawTexture(texture, RectInt{ destLeft, destTop, texture ? int(texture->GetWidth()) : 50, texture ? int(texture->GetHeight()) : 50 }, srcRect, opacity);
}
void Engine::DrawTexture(const Texture* const texture, const Point2Int& destLeftTop, const RectInt& srcRect, float opacity)const
{
    DrawTexture(texture, RectInt{ destLeftTop.x, destLeftTop.y, texture ? int(texture->GetWidth()) : 50, texture ? int(texture->GetHeight()) : 50 }, srcRect, opacity);
}
void Engine::DrawTexture(const Texture* const texture, const RectInt& destRect, const RectInt& srcRect, float opacity)const
    {
        D2D1_RECT_F destination = D2D1::RectF(
            static_cast<FLOAT>(destRect.left),
            static_cast<FLOAT>(destRect.top),
            static_cast<FLOAT>(destRect.left + destRect.width),
            static_cast<FLOAT>(destRect.top + destRect.height)
        );
    
        D2D1_RECT_F source{};
        if ((srcRect.width <= 0 || srcRect.height <= 0) && texture)
        {
            source = D2D1::RectF(
                0.F,
                0.F,
                static_cast<FLOAT>(texture->GetWidth()),
                static_cast<FLOAT>(texture->GetHeight())
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
        if (texture)
        {

            m_pDBitmapRenderTarget->DrawBitmap(
                texture->GetBitmap(),
                destination,
                opacity,
                D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
                source
            );
        }
        else
        {
            FillRectangle(destRect);
            OutputDebugString(_T("ERROR! Texture was nullptr in DrawTexture!\n"));
        }
    }
    //Ellipse
    void Engine::DrawEllipse(const Point2Int& center, float radiusX, float radiusY, float lineThickness)const
    {
        DrawEllipse(center.x, center.y, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawEllipse(const EllipseInt& ellipse, float lineThickness)const
    {
       DrawEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY, lineThickness);
    }
    void Engine::DrawEllipse(int centerX, int centerY, float radiusX, float radiusY, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(static_cast<FLOAT>(centerX), static_cast<FLOAT>(centerY)),
                static_cast<FLOAT>(radiusX),
                static_cast<FLOAT>(radiusY)),
            m_pDColorBrush,
            static_cast<FLOAT>(lineThickness));
    }

    //-----------------
    //Fill
    //-----------------
    
    //Rectangles
    void Engine::FillRectangle(const Point2Int& leftTop, int width, int height)const
    {
        FillRectangle(leftTop.x, leftTop.y, width, height);
    }
    void Engine::FillRectangle(const RectInt& rect)const
    {
        FillRectangle( rect.left,rect.top,rect.width,rect.height);
    }
    void Engine::FillRectangle(int left, int top, int width, int height)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillRectangle(
            D2D1::RectF(
                static_cast<FLOAT>(left),
                static_cast<FLOAT>(top),
                static_cast<FLOAT>(left + width),
                static_cast<FLOAT>(top + height)),
            m_pDColorBrush);
    }

    //RoundedRects
    void Engine::FillRoundedRect(const Point2Int& leftTop, int width, int height, float radiusX, float radiusY)const
    {
        FillRoundedRect(leftTop.x, leftTop.y, width, height, radiusX, radiusY);
    }
    void Engine::FillRoundedRect(const RectInt& rect, float radiusX, float radiusY)const
    {
        FillRoundedRect( rect.left, rect.top, rect.width, rect.height, radiusX, radiusY);
    }
    void Engine::FillRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillRoundedRectangle(
            D2D1::RoundedRect(
                D2D1::RectF(
                    static_cast<FLOAT>(left),
                    static_cast<FLOAT>(top),
                    static_cast<FLOAT>(left + width),
                    static_cast<FLOAT>(top + height)),
                static_cast<FLOAT>(radiusX),
                static_cast<FLOAT>(radiusY)),
            m_pDColorBrush);
        
    }

    void Engine::CreatePolygon(ID2D1PathGeometry* pGeo, const std::vector<Point2Int>& points, bool closeSegment) const
    {

        ID2D1GeometrySink* pSink;
        HRESULT hr = (pGeo)->Open(&pSink);

        if (SUCCEEDED(hr))
        {
            std::vector<D2D1_POINT_2F> D2points(points.size());

            for (size_t i = 0; i < points.size(); i++)
            {
                D2points[i] = D2D1::Point2F(static_cast<FLOAT>(points[i].x), static_cast<FLOAT>(points[i].y));
            }

            pSink->BeginFigure(D2points[0], D2D1_FIGURE_BEGIN_FILLED);
            for (size_t i = 1; i < points.size(); i++)
            {
                pSink->AddLine(D2points[i]);
            }
            pSink->EndFigure(closeSegment ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
            pSink->Close();
        }
        SafeRelease(&pSink);

    }
    void Engine::CreateArc(ID2D1PathGeometry* pGeo, const Point2Int& center, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment) const
    {

        ID2D1GeometrySink* pSink;
        HRESULT hr = (pGeo)->Open(&pSink);
        if(SUCCEEDED(hr))
        {

            auto startRad = (startAngle + (angle < 0.f ? angle : 0)) * M_PI / 180;
            auto endRad = (startAngle + (angle > 0.f ? angle : 0)) * M_PI / 180;

            auto beginPoint = D2D1::Point2F(
                static_cast<FLOAT>(center.x + radiusX * std::cos(startRad)),
                static_cast<FLOAT>(center.y + radiusY * std::sin(startRad))
            );
            auto endPoint = D2D1::Point2F(
                static_cast<FLOAT>(center.x + radiusX * std::cos(endRad)),
                static_cast<FLOAT>(center.y + radiusY * std::sin(endRad))
            );

            D2D1_ARC_SEGMENT arcSegment{
                endPoint,
                D2D1::SizeF(static_cast<FLOAT>(radiusX), static_cast<FLOAT>(radiusY)),
                0,
                D2D1_SWEEP_DIRECTION_CLOCKWISE,
                angle < 180.f ? D2D1_ARC_SIZE_SMALL : D2D1_ARC_SIZE_LARGE
            };



            pSink->BeginFigure(beginPoint, D2D1_FIGURE_BEGIN_FILLED);
            pSink->AddArc(arcSegment);
            if (closeSegment) pSink->AddLine(D2D1::Point2F(static_cast<FLOAT>(center.x), static_cast<FLOAT>(center.y)));
            pSink->EndFigure(closeSegment ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
            pSink->Close();
        }

        SafeRelease(&pSink);
    }

    //Ellipse
    void Engine::FillEllipse(const Point2Int& center, float radiusX, float radiusY)const
    {
        FillEllipse(center.x, center.y, radiusX, radiusY);
    }
    void Engine::FillEllipse(const EllipseInt& ellipse)const
    {
        FillEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY);
    }
    void Engine::FillEllipse(int centerX, int centerY, float radiusX, float radiusY)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(static_cast<FLOAT>(centerX), static_cast<FLOAT>(centerY)),
                static_cast<FLOAT>(radiusX),
                static_cast<FLOAT>(radiusY)),
            m_pDColorBrush);
    }
    #endif // MATHEMATICAL_COORDINATSYSTEM

    void Engine::DrawPolygon(const std::vector<Point2Int>& points, float lineThickness, bool closeSegment) const
    {
        ID2D1PathGeometry* pGeo{};
        
        HRESULT hr = m_pDFactory->CreatePathGeometry(&pGeo);

        if (SUCCEEDED(hr))
        {
            CreatePolygon(pGeo, points, closeSegment);

            SetTransform();

            m_pDBitmapRenderTarget->DrawGeometry(pGeo, m_pDColorBrush, static_cast<FLOAT>(lineThickness));
        }
        SafeRelease(&pGeo);
    }

    void Engine::FillPolygon(const std::vector<Point2Int>& points) const
    {
        ID2D1PathGeometry* pGeo{};

        HRESULT hr = m_pDFactory->CreatePathGeometry(&pGeo);


        if (SUCCEEDED(hr))
        {
            CreatePolygon(pGeo, points, true);
            SetTransform();
            m_pDBitmapRenderTarget->FillGeometry(pGeo, m_pDColorBrush);
        }

       SafeRelease(&pGeo);
    }


    void Engine::DrawArc(int centerX, int centerY, float radiusX, float radiusY, float startAngle, float angle, float lineThickness, bool closeSegment) const
    {
        DrawArc(Point2Int{ centerX, centerY }, radiusX, radiusY, startAngle, angle, lineThickness, closeSegment);
    }
    
    void Engine::DrawArc(const Point2Int& center, float radiusX, float radiusY, float startAngle, float angle, float lineThickness, bool closeSegment) const
    {
        if (angle >= 360.f)
        {
            angle = 359.9f;
            OutputDebugString(_T("Angle is larger or equal to 360. Use Ellipse instead.\n"));
        }
        if (angle <= -360.f)
        {
            angle = -359.9f;
            OutputDebugString(_T("Angle is smaller or equal to -360. Use Ellipse instead.\n"));
        }
        while (startAngle >= 360.f) startAngle -= 360;
        while (startAngle <= -360.f) startAngle += 360;

        ID2D1PathGeometry* pGeo{};
        HRESULT hr = m_pDFactory->CreatePathGeometry(&pGeo);
        if (SUCCEEDED(hr))
        {
            CreateArc(pGeo, center, radiusX, radiusY, startAngle, angle, closeSegment);

            SetTransform();

            m_pDBitmapRenderTarget->DrawGeometry(pGeo, m_pDColorBrush, static_cast<FLOAT>(lineThickness));
        }
        SafeRelease(&pGeo);
    }
    
    void Engine::FillArc(int centerX, int centerY, float radiusX, float radiusY, float startAngle, float angle) const
    {
        FillArc(Point2Int{ centerX, centerY }, radiusX, radiusY, startAngle, angle);
    }
    
    void Engine::FillArc(const Point2Int& center, float radiusX, float radiusY, float startAngle, float angle) const
    {
        if (angle >= 360.f)
        {
            angle = 359.9f;
            OutputDebugString(_T("Angle is larger or equal to 360. Use Ellipse instead.\n"));
        }
        if (angle <= -360.f)
        {
            angle = -359.9f;
            OutputDebugString(_T("Angle is smaller or equal to -360. Use Ellipse instead.\n"));
        }
        while (startAngle >= 360.f) startAngle -= 360;
        while (startAngle <= -360.f) startAngle += 360;

        ID2D1PathGeometry* pGeo{};
        HRESULT hr = m_pDFactory->CreatePathGeometry(&pGeo);
        if(SUCCEEDED(hr))
        {
            CreateArc(pGeo, center, radiusX, radiusY, startAngle, angle, true);
            SetTransform();
            m_pDBitmapRenderTarget->FillGeometry(pGeo, m_pDColorBrush);
        }
        SafeRelease(&pGeo);
    }
    
    
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
    void Engine::SetWindowDimensions(int width, int height, bool refreshWindowPos)
    {
        m_GameWidth = width;
        m_GameHeight = height;
        if(refreshWindowPos) SetWindowPosition();
    }
    void Engine::SetWindowScale(float scale)
    {
        m_WindowScale = scale;
    }
    void Engine::SetWindowPosition()
    {
        MONITORINFOEX mi{};
        mi.cbSize = sizeof(MONITORINFOEX);

        if (GetMonitorInfo(MonitorFromWindow(m_hWindow, MONITOR_DEFAULTTOPRIMARY), &mi))
        { 
            ::SetWindowLongPtr(m_hWindow, GWL_STYLE, m_OriginalStyle);

            UINT dpi = GetDpiForWindow(m_hWindow);

            m_WindowWidth = static_cast<int>(m_GameWidth * m_WindowScale);
            m_WindowHeight = static_cast<int>(m_GameHeight * m_WindowScale);

            int windowWidth{ (GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + m_WindowWidth + 10) };
            int windowHeight{ (GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
                                GetSystemMetrics(SM_CYCAPTION) + m_WindowHeight + 10) };
           
            windowWidth = static_cast<int>(windowWidth * dpi / 96.f);
            windowHeight = static_cast<int>(windowHeight * dpi / 96.f);
            
            int xPos{ mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left) / 2 - windowWidth / 2 };
            int yPos{ mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2 - windowHeight / 2 };

            ::SetWindowPos(m_hWindow, NULL, xPos, yPos, windowWidth, windowHeight, SWP_FRAMECHANGED);
        }
    
        if (m_pGame)
        {
            ::ShowWindow(m_hWindow, SW_SHOWNORMAL);
            ::UpdateWindow(m_hWindow);
        }
    }
    void Engine::SetFullscreen()
    {
        MONITORINFOEX mi{};
        mi.cbSize = sizeof(MONITORINFOEX);

        if(GetMonitorInfo(MonitorFromWindow(m_hWindow, MONITOR_DEFAULTTONEAREST), &mi))
        {
            //https://www.codeproject.com/Questions/108400/How-to-Set-Win32-Application-to-Full-Screen-C
            m_OriginalStyle = static_cast<DWORD>(::GetWindowLongPtr(m_hWindow, GWL_STYLE));
            DWORD dwRemove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
            DWORD dwNewStyle = m_OriginalStyle & ~dwRemove;
            ::SetWindowLongPtr(m_hWindow, GWL_STYLE, dwNewStyle);
            
            m_WindowWidth = static_cast<int>(mi.rcMonitor.right - mi.rcMonitor.left);
            m_WindowHeight = static_cast<int>(mi.rcMonitor.bottom - mi.rcMonitor.top);

            ::SetWindowPos(m_hWindow, NULL, mi.rcMonitor.left, mi.rcMonitor.top,
                m_WindowWidth,
                m_WindowHeight,
                SWP_FRAMECHANGED);
        }
        if (m_pGame)
        {
            ShowWindow(m_hWindow, SW_DENORMAL);
            UpdateWindow(m_hWindow);
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

    void Engine::SetTransform() const
    {
        if (m_TransformChanged)
        {
            D2D1::Matrix3x2F combinedMatrix{D2D1::Matrix3x2F::Identity()};
            for (const auto& matrix : m_VecTransformMatrices)
            {
                combinedMatrix = matrix * combinedMatrix;
            }
    
            m_pDBitmapRenderTarget->SetTransform(combinedMatrix);
    
            m_TransformChanged = false;
        }
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
            lastMatrix = D2D1::Matrix3x2F::Rotation(static_cast<FLOAT>(-angle), D2D1::Point2F(static_cast<FLOAT>(xPivotPoint), static_cast<FLOAT>(m_GameHeight - yPivotPoint))) * lastMatrix;
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
                D2D1::Point2F(static_cast<FLOAT>(xPointToScaleFrom), static_cast<FLOAT>(m_GameHeight - yPointToScaleFrom)))
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
    void Engine::Scale(float xScale, float yScale)
    {
        Scale(xScale, yScale, 0, 0);
    }
    void Engine::Scale(float scale)
    {
        Scale(scale, 0, 0);
    }
    
    void Engine::AddController()
    {
        if (m_pVecControllers.size() < 4)
        {
            m_pVecControllers.emplace_back(std::make_unique<Controller>(static_cast<uint8_t>(m_pVecControllers.size())));
        }
    #ifdef _DEBUG
        else OutputDebugString(_T( "Max amount of controllers already reached.\n"));
    #endif // _DEBUG
    }
    
    void Engine::PopController()
    {
        if (not m_pVecControllers.empty()) m_pVecControllers.pop_back();
    }
    
    void Engine::PopAllControllers()
    {
        m_pVecControllers.clear();
    }
    
    bool Engine::IsAnyButtonPressed() const
    {
        for (const auto& pController : m_pVecControllers)
        {
            if(pController->IsAnyButtonPressed()) return true;
        }
        return false;
    }
    
    bool Engine::ButtonDownThisFrame(Controller::Button button, uint8_t controllerIndex) const
    {
        return m_pVecControllers.at(controllerIndex)->IsDownThisFrame(button);
    }
    
    bool Engine::ButtonUpThisFrame(Controller::Button button, uint8_t controllerIndex) const
    {
        return m_pVecControllers.at(controllerIndex)->IsUpThisFrame(button);
    }
    
    bool Engine::ButtonPressed(Controller::Button button, uint8_t controllerIndex) const
    {
        return m_pVecControllers.at(controllerIndex)->IsPressed(button);
    }
    
    void Engine::ShowMouse(bool show)
    {
        ShowCursor(show);

        InvalidateRect(m_hWindow, NULL, true);
    }

    void Engine::SetFont(const Font* const pFont)
    {
        m_pResourceManager->SetCurrentFont(pFont);
    }

    void Engine::SetTextFormat(TextFormat* const pTextFormat)
    {
        m_pResourceManager->SetCurrentTextFormat(pTextFormat);
    }

    void Engine::SetColor(COLORREF newColor, float opacity)
    {
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
        HRESULT hr = OnRender();
    
        if (hr == D2DERR_RECREATE_TARGET)
        {
            hr = S_OK;
            SafeRelease(&m_pDBitmap);
            SafeRelease(&m_pDBitmapRenderTarget);
            SafeRelease(&m_pDRenderTarget);
            SafeRelease(&m_pDColorBrush);
        }
        ValidateRect(m_hWindow, NULL);
    }
    
    ResourceManager* const Engine::ResourceMngr() const
    {
        return m_pResourceManager.get();
    }

    const Font* const Engine::GetCurrentFont() const
    {
        return m_pResourceManager->GetCurrentFont();
    }

    RectInt Engine::GetWindowRect() const
    {
        return RectInt{ 0, 0, m_GameWidth, m_GameHeight };
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
    bool Engine::IsKeyBoardActive() const
    {
        return m_IsKeyboardActive;
    }
    ID2D1HwndRenderTarget* Engine::GetRenderTarget() const
    {
        return m_pDRenderTarget;
    }
    ID2D1BitmapRenderTarget* Engine::GetBitmapRenderTarget() const
    {
        return m_pDBitmapRenderTarget;
    }
 
    
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
        float lhs = (p.x * p.x) * (e.radiusY * e.radiusY) + (p.y * p.y) * (e.radiusX * e.radiusX);
        float rhs = (e.radiusX * e.radiusX) * (e.radiusY * e.radiusY);
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
}