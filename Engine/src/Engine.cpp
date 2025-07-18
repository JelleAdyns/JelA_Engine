
#include "Engine.h"
#include <algorithm>
#include <numbers>

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
        m_SecondsPerFrame{1.f/60.f},
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

            float xCoordinate = std::round(
                (GET_X_LPARAM(lParam) - m_ViewPortTranslationX) / 
                (m_WindowWidth - m_ViewPortTranslationX * 2) * m_GameWidth);

            float yCoordinate = std::round(
                (GET_Y_LPARAM(lParam) - m_ViewPortTranslationY) /
                (m_WindowHeight - m_ViewPortTranslationY * 2) *m_GameHeight);

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
                QueryPerformanceCounter(&m_TriggerCount);
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
    
                    m_WindowWidth = static_cast<int>(std::round(GetRenderTargetSize().width));
                    m_WindowHeight = static_cast<int>(std::round(GetRenderTargetSize().height));
    
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
        m_pGame = std::move(game);
        m_pGame->Initialize();

        SetWindowPosition();
     
        LARGE_INTEGER countsPersSecond, currentCount, lastCount;
        QueryPerformanceFrequency(&countsPersSecond);
        QueryPerformanceCounter(&currentCount);
        m_TriggerCount = currentCount;
        lastCount= currentCount;
        
        MSG msg{};
        bool playing = true;
        // Main message loop:
        while (playing)
        {
    
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    DestroyWindow(m_hWindow);
                    playing = false;
                }
    
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
          
            QueryPerformanceCounter(&currentCount);

            if (m_IsVSyncEnabled || currentCount.QuadPart >= m_TriggerCount.QuadPart)
            {
               SetDeltaTime(float(currentCount.QuadPart - lastCount.QuadPart) / countsPersSecond.QuadPart);
                lastCount = currentCount;

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

                m_TriggerCount.QuadPart = currentCount.QuadPart + int(m_SecondsPerFrame * countsPersSecond.QuadPart); 
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
                D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE),
                D2D1::HwndRenderTargetProperties(m_hWindow, size, m_IsVSyncEnabled ? D2D1_PRESENT_OPTIONS_NONE : D2D1_PRESENT_OPTIONS_IMMEDIATELY),
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
    void Engine::ResetRenderTargets()
    {
        SafeRelease(&m_pDBitmap);
        SafeRelease(&m_pDBitmapRenderTarget);
        SafeRelease(&m_pDRenderTarget);
        SafeRelease(&m_pDColorBrush);
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
                D2D1::RectF
                (
                    m_ViewPortTranslationX,
                    m_ViewPortTranslationY,
                    m_WindowWidth - m_ViewPortTranslationX,
                    m_WindowHeight - m_ViewPortTranslationY
                ),
                1.f,
                D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
            );
        }
        
        hr = m_pDRenderTarget->EndDraw();
        //-------------------------------------------------------
        
        return hr;
    }
    
    //lines
    
    void Engine::DrawLine(const Point2f& firstPoint, const Point2f& secondPoint, float lineThickness) const
    {
        DrawLine(firstPoint.x, firstPoint.y, secondPoint.x, secondPoint.y, lineThickness);
       
    }
    void Engine::DrawLine(float firstX, float firstY, const Point2f& secondPoint, float lineThickness) const
    {
        DrawLine(firstX, firstY, secondPoint.x, secondPoint.y, lineThickness);
    }
    
    void Engine::DrawLine(const Point2f& firstPoint, float secondX, float secondY, float lineThickness) const
    {
        DrawLine(firstPoint.x, firstPoint.y, secondX, secondY, lineThickness);
    }
    
    void Engine::DrawVector(const Point2f& origin, const Vector2f& vector, float headLineLength, float lineThickness) const
    {
        DrawVector(origin.x, origin.y, vector.x, vector.y, headLineLength, lineThickness);
    }
    void Engine::DrawVector(const Point2f& origin, float vectorX, float vectorY, float headLineLength, float lineThickness) const
    {
        DrawVector(origin.x, origin.y, vectorX, vectorY, headLineLength, lineThickness);
    }
    void Engine::DrawVector(float originX, float originY, const Vector2f& vector, float headLineLength, float lineThickness) const
    {
        DrawVector(originX, originY, vector.x, vector.y, headLineLength, lineThickness);
    }
    void Engine::DrawVector(float originX, float originY, float vectorX, float vectorY, float headLineLength, float lineThickness) const
    {
        SetTransform();
    
        const float endX = originX + vectorX;
        const float endY = originY + vectorY;
    
        const float desiredHeadAngle = std::numbers::pi_v<float> / 12.f;
        const float mirroredVectorAngle = atan2f(vectorY, vectorX) + std::numbers::pi_v<float>;
    
        const Point2f arrowP2{ endX + cosf(mirroredVectorAngle - desiredHeadAngle) * headLineLength,
                                endY + sinf(mirroredVectorAngle - desiredHeadAngle) * headLineLength };
    
        
        const Point2f arrowP3{ endX + cosf(mirroredVectorAngle + desiredHeadAngle) * headLineLength,
                                endY + sinf(mirroredVectorAngle + desiredHeadAngle) * headLineLength };
    
        DrawLine(originX, originY, endX, endY, lineThickness);
        DrawLine(endX, endY, arrowP2.x, arrowP2.y, lineThickness);
        DrawLine(endX, endY, arrowP3.x, arrowP3.y, lineThickness);
    }

    #ifdef MATHEMATICAL_COORDINATESYSTEM
    void Engine::DrawLine(float firstX, float firstY, float secondX, float secondY, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawLine(
            D2D1::Point2F(firstX, m_GameHeight - firstY),
            D2D1::Point2F(secondX, m_GameHeight - secondY),
            m_pDColorBrush,
            lineThickness
        );
    }
    
    
    //Rectangles

    void Engine::DrawRectangle(const Point2f& leftBottom, float width, float height, float lineThickness)const
    {
        DrawRectangle( leftBottom.x, leftBottom.y, width, height, lineThickness);
    }
    void Engine::DrawRectangle(const Rectf& rect, float lineThickness)const
    {
        DrawRectangle( rect.left, rect.bottom ,rect.width, rect.height, lineThickness);
    } 
    void Engine::DrawRectangle(float left, float bottom, float width, float height, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawRectangle(
            D2D1::RectF
            (
                left,
                m_GameHeight - (bottom + height),
                left + width,
                m_GameHeight - bottom
            ),
            m_pDColorBrush,
            lineThickness
        );
       
    }
    
    //RoundedRects
    void Engine::DrawRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY, float lineThickness)const
    {
        DrawRoundedRect(leftBottom.x, leftBottom.y, width, height, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness)const
    {
        DrawRoundedRect(rect.left, rect.bottom, rect.width, rect.height, radiusX, radiusY, lineThickness);
    }   
    void Engine::DrawRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawRoundedRectangle(
            D2D1::RoundedRect
            (
                D2D1::RectF
                (
                    left,
                    m_GameHeight - (bottom + height),
                    left + width,
                    m_GameHeight - bottom
                ),
                radiusX,
                radiusY
            ),
            m_pDColorBrush,
            lineThickness
        );
    }


    
    // Strings
    void Engine::DrawString(const tstring& textToDisplay, const Point2f& leftBottom, float width, float height, bool showRect)const
    {
        DrawString(textToDisplay, leftBottom.x, leftBottom.y, width, height, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, const Rectf& destRect, bool showRect)const
    {
        DrawString(textToDisplay, destRect.left, destRect.bottom, destRect.width, destRect.height, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, float left, float bottom, float width, float height, bool showRect)const
    {
        SetTransform();
        D2D1_RECT_F rect = D2D1::RectF(
            left,
            m_GameHeight - (bottom + height),
            left + width,
            m_GameHeight - bottom);
    
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

    void Engine::DrawString(const tstring& textToDisplay, const Point2f& leftBottom, float width, bool showRect)const
    {
       DrawString(textToDisplay, leftBottom.x, leftBottom.y, width, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, float left, float bottom, float width, bool showRect)const
    {
        SetTransform();
        D2D1_RECT_F rect = D2D1::RectF(
            left,
            m_GameHeight - (bottom + m_pResourceManager->GetCurrentTextFormat()->GetFontSize()),
            left + width,
            m_GameHeight - bottom);

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
    void Engine::DrawTexture(const Texture* const texture, float destLeft, float destBottom, const Rectf& srcRect, float opacity)const
    {
        DrawTexture(texture, Rectf{ destLeft, destBottom, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void Engine::DrawTexture(const Texture* const texture, const Point2f& destLeftBottom, const Rectf& srcRect, float opacity)const
    {
        DrawTexture(texture, Rectf{ destLeftBottom.x, destLeftBottom.y, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void Engine::DrawTexture(const Texture* const texture, const Rectf& destRect, const Rectf& srcRect, float opacity)const
    {
        Rectf wndwSize = GetWindowRect();

        D2D1_RECT_F destination = D2D1::RectF(
            destRect.left,
            wndwSize.height - (destRect.bottom + destRect.height),
            destRect.left + destRect.width,
            wndwSize.height - destRect.bottom
        );
        D2D1_RECT_F source{};
        if ((srcRect.width <= 0 || srcRect.height <= 0) && texture)
        {
            source = D2D1::RectF(0.F,0.F,texture->GetWidth(),texture->GetHeight());
        }
        else
        {
            float sliceMargin{ 0.07f };
            source = D2D1::RectF(
                srcRect.left + sliceMargin,
                srcRect.bottom + sliceMargin,
                srcRect.left + srcRect.width - sliceMargin,
                srcRect.bottom + srcRect.height - sliceMargin);
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
    void Engine::DrawEllipse(const Point2f& center, float radiusX, float radiusY, float lineThickness)const
    {
        DrawEllipse(center.x, center.y, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawEllipse(const Ellipsef& ellipse, float lineThickness)const
    {
        DrawEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY, lineThickness);
    }
    void Engine::DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(centerX, m_GameHeight - centerY),
                radiusX, radiusY
            ),
            m_pDColorBrush,
            lineThickness
        );

    }
    void Engine::DrawCircle(const Circlef& circle, float lineThickness) const
    {
        DrawEllipse(circle.center, circle.rad, circle.rad, lineThickness);
    }

    //-----------------
    //Fill
    //----------------

    //Rectangles
    void Engine::FillRectangle(const Point2f& leftBottom, float width, float height)const
    {
        FillRectangle(leftBottom.x, leftBottom.y, width, height);
    }
    void Engine::FillRectangle(const Rectf& rect)const
    {
        FillRectangle(rect.left, rect.bottom, rect.width, rect.height);
    }
    void Engine::FillRectangle(float left, float bottom, float width, float height)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillRectangle(
            D2D1::RectF(
                left,
                m_GameHeight - (bottom + height),
                left + width,
                m_GameHeight - bottom
            ),
            m_pDColorBrush);

    }

    //RoundedRects
    void Engine::FillRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillRoundedRectangle(
            D2D1::RoundedRect(
                D2D1::RectF(
                    left,
                    m_GameHeight - (bottom + height),
                    left + width,
                    m_GameHeight - bottom
                ),
                radiusX,
                radiusY
            ),
            m_pDColorBrush);
    }
    void Engine::FillRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY)const
    {
        FillRoundedRect(leftBottom.x, leftBottom.y, width, height, radiusX, radiusY);
    }
    void Engine::FillRoundedRect(const Rectf& rect, float radiusX, float radiusY)const
    {
        FillRoundedRect(rect.left, rect.bottom, rect.width, rect.height, radiusX, radiusY);
    }

    //Ellipses
    void Engine::FillEllipse(float centerX, float centerY, float radiusX, float radiusY)const
    {
        FillEllipse(Ellipsef{ centerX, centerY, radiusX, radiusY });
    }
    void Engine::FillEllipse(const Point2f& center, float radiusX, float radiusY)const
    {
        FillEllipse(Ellipsef{ center, radiusX, radiusY });
    }
    void Engine::FillEllipse(const Ellipsef& ellipse)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(ellipse.center.x, m_GameHeight - ellipse.center.y),
                ellipse.radiusX, ellipse.radiusY
            ),
            m_pDColorBrush);
    }
    void Engine::FillCircle(const Circlef& circle) const
    {
        FillEllipse(circle.center, circle.rad, circle.rad);
    }
#else

    //Lines
    void Engine::DrawLine(float firstX, float firstY, float secondX, float secondY, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawLine(
            D2D1::Point2F(firstX, firstY), D2D1::Point2F(secondX, secondY),
            m_pDColorBrush,
            lineThickness
        );
    }
    
    
    //Rectangles
    
    void Engine::DrawRectangle(const Point2f& leftTop, float width, float height, float lineThickness)const
    {
        DrawRectangle(leftTop.x, leftTop.y, width, height, lineThickness);
    }
    void Engine::DrawRectangle(const Rectf& rect, float lineThickness)const
    {
        DrawRectangle(rect.left, rect.top, rect.width, rect.height, lineThickness);
    }
    void Engine::DrawRectangle(float left, float top, float width, float height, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawRectangle(
            D2D1::RectF(left, top, left + width, top + height),
            m_pDColorBrush,
            lineThickness
        );
    }
    
    //RoundedRects
    void Engine::DrawRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY, float lineThickness)const
    {
        DrawRoundedRect(leftTop.x, leftTop.y, width, height, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness)const
    {
        DrawRoundedRect(rect.left, rect.top, rect.width, rect.height, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawRoundedRectangle(
            D2D1::RoundedRect(
                D2D1::RectF(left, top, left + width, top + height),
                radiusX,radiusY
            ),
            m_pDColorBrush,
            lineThickness
        );
    }
    
    //String  
    void Engine::DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, float height, bool showRect)const
    {
        DrawString(textToDisplay, leftTop.x, leftTop.y, width, height, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, const Rectf& destRect, bool showRect)const
    {
        DrawString(textToDisplay, destRect.left, destRect.top, destRect.width, destRect.height, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, float left, float top, float width, float height, bool showRect)const
    {
        SetTransform();
        D2D1_RECT_F rect = D2D1::RectF(left, top, left + width, top + height);
    
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
    void Engine::DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, bool showRect)const
    {
        DrawString(textToDisplay, leftTop.x, leftTop.y, width, showRect);
    }
    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void Engine::DrawString(const tstring& textToDisplay, float left, float top, float width, bool showRect)const
    {
        SetTransform();
        D2D1_RECT_F rect = D2D1::RectF(left, top, left + width, top + m_pResourceManager->GetCurrentTextFormat()->GetFontSize());
    
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
    void Engine::DrawTexture(const Texture* const texture, float destLeft, float destTop, const Rectf& srcRect, float opacity)const
    {
        DrawTexture(texture, Rectf{ destLeft, destTop, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void Engine::DrawTexture(const Texture* const texture, const Point2f& destLeftTop, const Rectf& srcRect, float opacity)const
    {
        DrawTexture(texture, Rectf{ destLeftTop.x, destLeftTop.y, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void Engine::DrawTexture(const Texture* const texture, const Rectf& destRect, const Rectf& srcRect, float opacity)const
    {
        D2D1_RECT_F destination = D2D1::RectF(destRect.left, destRect.top, destRect.left + destRect.width, destRect.top + destRect.height);
    
        D2D1_RECT_F source{};
        if ((srcRect.width <= 0 || srcRect.height <= 0) && texture)
        {
            source = D2D1::RectF(0.F, 0.F, texture->GetWidth(), texture->GetHeight());
        }
        else
        {
            float sliceMargin{ 0.07f };
            source = D2D1::RectF(
                srcRect.left + sliceMargin,
                srcRect.top + sliceMargin, 
                srcRect.left + srcRect.width - sliceMargin,
                srcRect.top + srcRect.height - sliceMargin
            );
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
    void Engine::DrawEllipse(const Point2f& center, float radiusX, float radiusY, float lineThickness)const
    {
        DrawEllipse(center.x, center.y, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawEllipse(const Ellipsef& ellipse, float lineThickness)const
    {
       DrawEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY, lineThickness);
    }
    void Engine::DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineThickness)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->DrawEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(centerX, centerY),
                radiusX, radiusY
            ),
            m_pDColorBrush,
            lineThickness
        );
    }

    //-----------------
    //Fill
    //-----------------
    
    //Rectangles
    void Engine::FillRectangle(const Point2f& leftTop, float width, float height)const
    {
        FillRectangle(leftTop.x, leftTop.y, width, height);
    }
    void Engine::FillRectangle(const Rectf& rect)const
    {
        FillRectangle( rect.left,rect.top,rect.width,rect.height);
    }
    void Engine::FillRectangle(float left, float top, float width, float height)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillRectangle(
            D2D1::RectF(left, top, left + width, top + height),
            m_pDColorBrush);
    }

    //RoundedRects
    void Engine::FillRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY)const
    {
        FillRoundedRect(leftTop.x, leftTop.y, width, height, radiusX, radiusY);
    }
    void Engine::FillRoundedRect(const Rectf& rect, float radiusX, float radiusY)const
    {
        FillRoundedRect( rect.left, rect.top, rect.width, rect.height, radiusX, radiusY);
    }
    void Engine::FillRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillRoundedRectangle(
            D2D1::RoundedRect(
                D2D1::RectF(left, top, left + width, top + height),
                radiusX, radiusY
            ),
            m_pDColorBrush
        );
    }

    //Ellipse
    void Engine::FillEllipse(const Point2f& center, float radiusX, float radiusY)const
    {
        FillEllipse(center.x, center.y, radiusX, radiusY);
    }
    void Engine::FillEllipse(const Ellipsef& ellipse)const
    {
        FillEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY);
    }
    void Engine::FillEllipse(float centerX, float centerY, float radiusX, float radiusY)const
    {
        SetTransform();
        m_pDBitmapRenderTarget->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(centerX, centerY), radiusX, radiusY),
            m_pDColorBrush);
    }
    #endif // MATHEMATICAL_COORDINATSYSTEM

    void Engine::DrawPolygon(const Polygon& polygon, float lineThickness)
    {
        DrawGeometry(&polygon, lineThickness);
    }
   
    void Engine::FillPolygon(const Polygon& polygon)
    {
        FillGeometry(&polygon);
    }

    void Engine::DrawArc(const Arc& arc, float lineThickness)
    {
        DrawGeometry(&arc, lineThickness);
    }

    void Engine::FillArc(const Arc& arc)
    {
        FillGeometry(&arc);
    }
    
    //Geometry
    void Engine::DrawGeometry(const Geometry* const pGeometryObject, float lineThickness)
    {
        PushTransform();
        Translate(pGeometryObject->GetTranslation());
        SetTransform();
        m_pDBitmapRenderTarget->DrawGeometry(pGeometryObject->GetGeometry(), m_pDColorBrush, lineThickness);
        PopTransform();
    }
    void Engine::FillGeometry(const Geometry* const pGeometryObject)
    {
        PushTransform();
        Translate(pGeometryObject->GetTranslation());
        SetTransform();
        m_pDBitmapRenderTarget->FillGeometry(pGeometryObject->GetGeometry(), m_pDColorBrush);
        PopTransform();
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
        m_SecondsPerFrame = 1.f / FPS;
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
    void Engine::Translate(float xTranslation, float yTranslation)
    {
        if (!m_VecTransformMatrices.empty())
        {
            auto& lastMatrix = m_VecTransformMatrices.back();
            lastMatrix = D2D1::Matrix3x2F::Translation(xTranslation, -yTranslation) * lastMatrix;
        }
        else OutputDebugString(_T("Vector of matrices was empty while trying to add a Translation matrix."));
    
        m_TransformChanged = true;
    }
    
    void Engine::Rotate(float angle, float xPivotPoint, float yPivotPoint)
    {
        if (!m_VecTransformMatrices.empty())
        {
            auto& lastMatrix = m_VecTransformMatrices.back();
            lastMatrix = D2D1::Matrix3x2F::Rotation(-angle, D2D1::Point2F(xPivotPoint, m_GameHeight - yPivotPoint)) * lastMatrix;
        }
        else OutputDebugString(_T("Vector of matrices was empty while trying to add a Rotation matrix."));
    
        m_TransformChanged = true;
    }
    void Engine::Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom)
    {
        if (!m_VecTransformMatrices.empty())
        {
            auto& lastMatrix = m_VecTransformMatrices.back();
            lastMatrix = D2D1::Matrix3x2F::Scale(xScale, yScale,
                D2D1::Point2F(xPointToScaleFrom, m_GameHeight - yPointToScaleFrom))
                * lastMatrix;
        }
        else OutputDebugString(_T("Vector of matrices was empty while trying to add a Scaling matrix."));
    
        m_TransformChanged = true;
    }
    #else
    void Engine::Translate(float xTranslation, float yTranslation)
    {
        if (!m_VecTransformMatrices.empty())
        {
            auto& lastMatrix = m_VecTransformMatrices.back();
            lastMatrix = D2D1::Matrix3x2F::Translation(xTranslation, yTranslation) * lastMatrix;
        }
        else OutputDebugString(_T("Vector of matrices was empty while trying to add a Translation matrix."));
    
        m_TransformChanged = true;
    }
    void Engine::Rotate(float angle, float xPivotPoint, float yPivotPoint)
    {
        if (!m_VecTransformMatrices.empty())
        {
            auto& lastMatrix = m_VecTransformMatrices.back();
            lastMatrix = D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(xPivotPoint, yPivotPoint)) * lastMatrix;
        }
        else OutputDebugString(_T("Vector of matrices was empty while trying to add a Rotation matrix."));
    }
    void Engine::Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom)
    {
        if (!m_VecTransformMatrices.empty())
        {
            auto& lastMatrix = m_VecTransformMatrices.back();
            lastMatrix = D2D1::Matrix3x2F::Scale(xScale, yScale, D2D1::Point2F(xPointToScaleFrom, yPointToScaleFrom)) * lastMatrix;
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
        Translate(translation.x, translation.y);
    }
    void Engine::Rotate(float angle, const Point2f& pivotPoint)
    {
        Rotate(angle, pivotPoint.x, pivotPoint.y);
    }
    void Engine::Scale(float scale, float xPointToScaleFrom, float yPointToScaleFrom)
    {
        Scale(scale, scale, xPointToScaleFrom, yPointToScaleFrom);
    }
    void Engine::Scale(float xScale, float yScale, const Point2f& PointToScaleFrom)
    {
        Scale(xScale, yScale, PointToScaleFrom.x, PointToScaleFrom.y);
    }
    void Engine::Scale(float scale, const Point2f& PointToScaleFrom)
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
    void Engine::SetVSync(bool enable)
    {
        m_IsVSyncEnabled = enable;
        ResetRenderTargets();
        CreateRenderTargets();
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
    
        m_pDColorBrush->SetOpacity(opacity);
    }
    void Engine::SetBackGroundColor(COLORREF newColor)
    {
        m_DColorBackGround = D2D1::ColorF(
            GetRValue(newColor) / 255.f,
            GetGValue(newColor) / 255.f,
            GetBValue(newColor) / 255.f,
            1.f);
    }
    
    Rectf Engine::GetRenderTargetSize() const
    {
        D2D1_SIZE_F size = m_pDRenderTarget->GetSize();
        return Rectf{ 0,0,size.width, size.height};
    }
    void Engine::Paint()
    {
        HRESULT hr = OnRender();
    
        if (hr == D2DERR_RECREATE_TARGET)
        {
            hr = S_OK;
            ResetRenderTargets();
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

    Rectf Engine::GetWindowRect() const
    {
        return Rectf{ 0, 0, static_cast<float>(m_GameWidth), static_cast<float>(m_GameHeight) };
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
    ID2D1Factory* Engine::GetFactory() const
    {
        return m_pDFactory;
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
    
    float utils::Distance(float x1, float y1, float x2, float y2)
    {
        const float b = x2 - x1;
        const float c = y2 - y1;
        return sqrt(b * b + c * c);
    }
    
    float utils::Distance(const Point2f& p1, const Point2f& p2)
    {
        return Distance(p1.x, p1.y, p2.x, p2.y);
    }
    
    bool utils::IsPointInRect(const Point2f& p, const Rectf& r)
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
    
    bool utils::IsPointInCircle(const Point2f& p, const Circlef& c)
    {
        const float x = c.center.x - p.x;
        const float y = c.center.y - p.y;
        return x * x + y * y < c.rad * c.rad;
    }
    
    bool utils::IsPointInEllipse(const Point2f& p, const Ellipsef& e)
    {
        const float xDist = p.x - e.center.x;
        const float yDist = p.y - e.center.y;
        const float xRadSqrd = e.radiusX * e.radiusX;
        const float yRadSqrd = e.radiusY * e.radiusY;

        const float lhs = xDist * xDist * yRadSqrd  +  yDist * yDist * xRadSqrd;
        const float rhs = xRadSqrd * yRadSqrd;

        return lhs <= rhs;
    }
    
    bool utils::IsOverlapping(const Point2f& a, const Point2f& b, const Circlef& c)
    {
        return DistPointLineSegment(c.center, a, b) <= c.rad;
    }
    
    bool utils::IsOverlapping(const Point2f& a, const Point2f& b, const Ellipsef& e)
    {
        return IsPointInEllipse(ClosestPointOnLine(e.center, a, b), e);
    }
    
    bool utils::IsOverlapping(const Point2f& a, const Point2f& b, const Rectf& r)
    {
        std::pair<Point2f, Point2f> p{};
        return utils::IntersectRectLine(r, a, b, p);
    }
    
    bool utils::IsOverlapping(const Rectf& r1, const Rectf& r2)
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
    
    bool utils::IsOverlapping(const Rectf& r, const Circlef& c)
    {
        if (IsPointInRect(c.center, r)) return true;
    
        float right = r.left + r.width;
    
    #ifdef MATHEMATICAL_COORDINATESYSTEM
        float top = r.bottom + r.height;
        if (DistPointLineSegment(c.center, Point2f{ r.left, r.bottom }, Point2f{ r.left, top }) <= c.rad) return true;
        
        if (DistPointLineSegment(c.center, Point2f{ r.left, r.bottom }, Point2f{ right, r.bottom }) <= c.rad) return true;
        
        if (DistPointLineSegment(c.center, Point2f{ r.left, top }, Point2f{ right, top }) <= c.rad) return true;
        
        if (DistPointLineSegment(c.center, Point2f{ right, top }, Point2f{ right, r.bottom }) <= c.rad) return true;
    #else
        float bottom = r.top + r.height;
        if (DistPointLineSegment(c.center, Point2f{ r.left, r.top }, Point2f{ r.left, bottom }) <= c.rad) return true;
        
        if (DistPointLineSegment(c.center, Point2f{ r.left, r.top }, Point2f{ right, r.top }) <= c.rad) return true;
        
        if (DistPointLineSegment(c.center, Point2f{ r.left, bottom }, Point2f{ right, bottom }) <= c.rad) return true;
        
        if (DistPointLineSegment(c.center, Point2f{ right, bottom }, Point2f{ right, r.top }) <= c.rad) return true;
        
    #endif // MATHEMATICAL_COORDINATESYSTEM
    
        return false;
    }
    
    bool utils::IsOverlapping(const Circlef& c1, const Circlef& c2)
    {
        return (c2.center - c1.center).SquaredLength() < (c1.rad + c2.rad) * (c1.rad + c2.rad);
    }
    
    
    Point2f utils::ClosestPointOnLine(const Point2f& point, const Point2f& linePointA, const Point2f& linePointB)
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
        Point2f intersection{ linePointA + pointProjectionOnLine * abNorm };
        return intersection;
    }
    
    float utils::DistPointLineSegment(const Point2f& point, const Point2f& linePointA, const Point2f& linePointB)
    {
        return (point - ClosestPointOnLine(point, linePointA, linePointB)).Length();
    }
    
    bool utils::IsPointOnLineSegment(const Point2f& point, const Point2f& linePointA, const Point2f& linePointB)
    {
        Vector2f aToPoint{ linePointA, point };
        Vector2f bToPoint{ linePointB, point };
    
        // If not on same line, return false
        if (abs(Vector2f::Cross(aToPoint, bToPoint)) > FLT_EPSILON) return false;
    
        // Both vectors must point in opposite directions if p is between a and b
        if (Vector2f::Dot(aToPoint, bToPoint) > 0) return false;
    
        return true;
    }
    bool utils::IntersectLines(const Vector2f& l1, const Vector2f& l2, const Point2f& origin1, const Point2f& origin2)
    {
        float crossArea = Vector2f::Cross(l1, l2);
        if (std::abs(crossArea) <= FLT_EPSILON) // if parallel
        {
            Vector2f OriginToOrigin{ origin1, origin2 };
            // if there's an offset, return false
            if (std::abs(Vector2f::Cross(OriginToOrigin, l2)) > FLT_EPSILON) return false;
        }
        return true;
    }
    bool utils::IntersectLines(const Point2f& p1, const Point2f& p2, const Point2f& q1, const Point2f& q2)
    {
        return IntersectLines({ p1,p2 }, { q1,q2 }, p1, q1);
    }
    bool utils::IntersectLineSegments(const Point2f& p1, const Point2f& p2, const Point2f& q1, const Point2f& q2, float& line1Interpolation, float& line2Interpolation)
    {
        if (!IntersectLines(p1, p2, q1, q2)) return false;
        
        bool intersecting{ false };
        Vector2f firstLine{ p1, p2 };
        Vector2f secondLine{ q1, q2 };
            
        float crossArea = Vector2f::Cross(firstLine, secondLine);

        if (std::abs(crossArea) <= FLT_EPSILON) // if parallel
        {
            line1Interpolation = 0;
            line2Interpolation = 0;
            if (utils::IsPointOnLineSegment(p1, q1, q2) ||
                utils::IsPointOnLineSegment(p2, q1, q2))
            {
                intersecting = true;
            }
        }
        else
        {
            Vector2f p1q1{ p1, q1 };
            float num1 = Vector2f::Cross(p1q1, secondLine);
            float num2 = Vector2f::Cross(p1q1, firstLine);

            line1Interpolation = num1 / crossArea;
            line2Interpolation = num2 / crossArea;
            
            if (line1Interpolation > 0 && line1Interpolation <= 1 && line2Interpolation > 0 && line2Interpolation <= 1)
                intersecting = true;
            
        }
        
        return intersecting;
    }
    
    bool utils::IntersectRectLine(const Rectf& r, const Point2f& p1, const Point2f& p2, std::pair<Point2f, Point2f>& intersections)
    {
        float xDenom{ p2.x - p1.x };
        float x1{ (r.left - p1.x) / xDenom};
        float x2{ (r.left + r.width - p1.x) / xDenom };
    
        float yDenom{ p2.y - p1.y };
    #ifdef MATHEMATICAL_COORDINATESYSTEM
        float y1{ (r.bottom - p1.y  ) / yDenom };
        float y2{ (r.bottom + r.height - p1.y ) / yDenom };
    #else  
        float y1{ (r.top - p1.y) / yDenom };
        float y2{ (r.top + r.height- p1.y) / yDenom };
    #endif // !MATHEMATICAL_COORDINATESYSTEM
        
    
        float tMin{ std::max(std::min(x1,x2), std::min(y1,y2)) };
        float tMax{ std::min(std::max(x1,x2), std::max(y1,y2)) };
    
        if (tMin > tMax) return false;
    
        Vector2f lineDirection{ p1, p2 };
        intersections.first = p1 + lineDirection * tMin;
        intersections.second = p1 + lineDirection * tMax;
    
        return true;
    }

    //---------------------------------------------------------------------------------------------------------------------------------
}