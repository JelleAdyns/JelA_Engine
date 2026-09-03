#include "Engine.h"
#include <algorithm>
#include <numbers>
#include <thread>

#include "AudioService.h"

namespace jela
{
    bool ContainsMouseButtons(jela::MouseButtons collectionOfButtons, jela::MouseButtons buttonsToCheckFor)
    {
        const auto buttonsToCheckValue = static_cast<int>(buttonsToCheckFor);
        return (static_cast<int>(collectionOfButtons) & buttonsToCheckValue) == buttonsToCheckValue;
    }
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ENGINE.HandleMessages(hWnd, message, wParam, lParam);
    }

    Engine::Engine() :
        m_hWindow{nullptr},
        m_hInstance{nullptr},
        m_OriginalStyle{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX },
        m_pGame{ nullptr },
        m_Title{ _T("Standard Game")},
        m_GameWidth{500},
        m_GameHeight{500},
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

        m_GameWidth = width;
        m_GameHeight = height;
        m_WindowWidth = width;
        m_WindowHeight = height;
        m_ViewPortWidth = static_cast<float>(width);
        m_ViewPortHeight = static_cast<float>(height);

        m_Title = wndwName;
        SetFrameRate(60);

        m_pResourceManager = std::make_unique<ResourceManager>(resourcePath);

        MakeWindow();
        // HResultHandler hr{ S_OK, _T("ENGINE::Init") };
        // hr = CreateRenderTargets();

        m_pDXHandler = std::make_unique<DX::DXHandler>();
        if (!m_pDXHandler->IsValid()) return false;
        SetBackGroundColor(bgColor);

        m_pResourceManager->Start();

        m_pSceneManager = std::make_unique<SceneManager>();

        return true;
    }

    int Engine::Run(std::unique_ptr<BaseGame>&& game)
    {

        m_pGame = std::move(game);
        m_pGame->Initialize();

        SetWindowPosition(true, true);

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
                    DestroyWindow(m_hWindow);
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

                if (IsAnyControllerButtonPressed()) m_IsKeyboardActive = false;

                for (const auto& controller : m_pVecControllers)
                    controller->ProcessControllerInput();

                if (!m_IsKeyboardActive)
                    m_pGame->HandleControllerInput();

                m_pGame->Tick();
                InvalidateRect(m_hWindow, nullptr, FALSE);
                //Paint();

                m_TriggerCount.QuadPart = currentCount.QuadPart + static_cast<int>(m_SecondsPerFrame * countsPersSecond.QuadPart);
            }
        }

        return static_cast<int>(msg.wParam);
    }

    void Engine::Shutdown()
    {
        if (m_pGame)
        {
            m_pGame->Cleanup();
            m_pGame = nullptr;
        }
        AudioLocator::RegisterAudioService(nullptr);

        m_pSceneManager = nullptr;
        m_pResourceManager = nullptr;
        m_pDXHandler = nullptr;

        CoUninitialize();
    }

    LRESULT Engine::HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        LRESULT result = 0;

        bool wasHandled = false;
        if (m_pGame)
        {
            float xCoordinate = (GET_X_LPARAM(lParam) - m_ViewPortTranslationX) / m_ViewPortWidth * m_GameWidth;
            xCoordinate = std::round(xCoordinate);
            float yCoordinate = (GET_Y_LPARAM(lParam) - m_ViewPortTranslationY) / m_ViewPortHeight * m_GameHeight;

            if (USE_MATHEMATICAL_COORDINATESYSTEM)
                yCoordinate = m_GameHeight - yCoordinate;

            yCoordinate = std::round(yCoordinate);

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
                const UINT width = LOWORD(lParam);
                const UINT height = HIWORD(lParam);
                if (m_pDXHandler->dSwapChain.IsInValidState())
                {
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

                    ResizeWindow();

                    CalculateWindowPos();
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
            case WM_MOVE:
            {
                CalculateWindowPos();
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_KEYUP:
            {
                if (static_cast<int>(wParam) == VK_F11)
                {
                    if (m_IsFullscreen) SetWindowPosition(true, true);
                    else SetFullscreen();
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

            case WM_LBUTTONDBLCLK:
                m_pGame->MouseDoubleClick(MouseButtons::Left, xCoordinate, yCoordinate, static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
                break;
            case WM_LBUTTONDOWN:
                m_pGame->MouseDown(MouseButtons::Left, xCoordinate, yCoordinate, static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
                break;
            case WM_LBUTTONUP:
                m_pGame->MouseUp(MouseButtons::Left, xCoordinate, yCoordinate, static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
                break;
            case WM_RBUTTONDBLCLK:
                m_pGame->MouseDoubleClick(MouseButtons::Right, xCoordinate, yCoordinate, static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
                break;
            case WM_RBUTTONDOWN:
                m_pGame->MouseDown(MouseButtons::Right, xCoordinate, yCoordinate, static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
                break;
            case WM_RBUTTONUP:
                m_pGame->MouseUp(MouseButtons::Right, xCoordinate, yCoordinate, static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
                break;
            case WM_MBUTTONDBLCLK:
                m_pGame->MouseDoubleClick(MouseButtons::Middle, xCoordinate, yCoordinate, static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
                break;
            case WM_MBUTTONDOWN:
                m_pGame->MouseDown(MouseButtons::Middle, xCoordinate, yCoordinate, static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
                break;
            case WM_MBUTTONUP:
                m_pGame->MouseUp(MouseButtons::Middle, xCoordinate, yCoordinate, static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
                break;
            case WM_MOUSEMOVE:
                m_pGame->MouseMove(xCoordinate, yCoordinate, static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
                break;
            case WM_MOUSEWHEEL:
            {
                float screenPosX = (GET_X_LPARAM(lParam) - m_WindowPosX - m_ViewPortTranslationX);
                if (!m_IsFullscreen) screenPosX -= GetSystemMetrics(SM_CXFIXEDFRAME) + m_WindowPosOffset;

                float xWheelCoordinate = screenPosX / m_ViewPortWidth * m_GameWidth;
                xWheelCoordinate = std::round(xWheelCoordinate);

                float screenPosY = (GET_Y_LPARAM(lParam) - m_WindowPosY - m_ViewPortTranslationY);
                if (!m_IsFullscreen) screenPosY -= GetSystemMetrics(SM_CXFIXEDFRAME) + m_WindowPosOffset + GetSystemMetrics(SM_CYCAPTION);

                float yWheelCoordinate = screenPosY / m_ViewPortHeight * m_GameHeight;

                if (USE_MATHEMATICAL_COORDINATESYSTEM)
                    yWheelCoordinate = m_GameHeight - yWheelCoordinate;

                yWheelCoordinate = std::round(yWheelCoordinate);

                m_pGame->MouseWheelTurn(xWheelCoordinate, yWheelCoordinate, GET_WHEEL_DELTA_WPARAM(wParam), static_cast<MouseButtons>(GET_KEYSTATE_WPARAM(wParam)));
                result = 0;
                wasHandled = true;
            }
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                m_IsQuitting = true;
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

    void Engine::Quit()
    {
        PostMessage(GetWindow(), WM_DESTROY, NULL, NULL);
        m_IsQuitting = true;
    }
    void Engine::MakeWindow()
    {
        WNDCLASSEX wcex{};

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = m_hInstance;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = m_Title.c_str();

        //https://stackoverflow.com/questions/77638632/how-to-add-an-icon-to-a-win32api-window-in-a-cmake-project
        const auto hIcon = static_cast<HICON>(LoadImage(
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

    }

    HResultHandler Engine::OnRender() const
    {
        HResultHandler hr{S_OK, _T("ENGINE::OnRender")};

        hr = m_pDXHandler->dDeviceContext2D.Draw([pGame = m_pGame.get()] { pGame->Draw(); });
        hr = m_pDXHandler->dSwapChain.Present();

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
    void Engine::DrawLine(float firstX, float firstY, float secondX, float secondY, float lineThickness) const
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM)
        {
            firstY = m_GameHeight - firstY;
            secondY = m_GameHeight - secondY;
        }

        m_pDXHandler->dDeviceContext2D.DrawLine(firstX, firstY, secondX, secondY, lineThickness);
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
        const float endX = originX + vectorX;
        const float endY = originY + vectorY;

        constexpr float desiredHeadAngle = std::numbers::pi_v<float> / 12.f;
        const float flippedVectorAngle = atan2f(-vectorY, -vectorX);

        const Point2f arrowP2{ endX + cosf(flippedVectorAngle - desiredHeadAngle) * headLineLength,
                                endY + sinf(flippedVectorAngle - desiredHeadAngle) * headLineLength };

        const Point2f arrowP3{ endX + cosf(flippedVectorAngle + desiredHeadAngle) * headLineLength,
                                endY + sinf(flippedVectorAngle + desiredHeadAngle) * headLineLength };

        DrawLine(originX, originY, endX, endY, lineThickness);
        DrawLine(endX, endY, arrowP2.x, arrowP2.y, lineThickness);
        DrawLine(endX, endY, arrowP3.x, arrowP3.y, lineThickness);
    }


#ifdef MATHEMATICAL_COORDINATESYSTEM

    //Rectangles
    void Engine::DrawRectangle(const Point2f& leftBottom, float width, float height, float lineThickness) const
    {
        DrawRectangle( leftBottom.x, leftBottom.y, width, height, lineThickness);
    }
    void Engine::DrawRectangle(const Rectf& rect, float lineThickness) const
    {
        DrawRectangle( rect.left, rect.bottom ,rect.width, rect.height, lineThickness);
    }

    void Engine::DrawRectangle(float left, float bottom, float width, float height, float lineThickness) const
    {
        m_pDXHandler->dDeviceContext2D.DrawRectangle(
            left,         m_GameHeight - (bottom + height),
            left + width, m_GameHeight - bottom,
            lineThickness);
    }

    //RoundedRects
    void Engine::DrawRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY, float lineThickness) const
    {
        DrawRoundedRect(leftBottom.x, leftBottom.y, width, height, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness) const
    {
        DrawRoundedRect(rect.left, rect.bottom, rect.width, rect.height, radiusX, radiusY, lineThickness);
    }

    void Engine::DrawRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY, float lineThickness) const
    {
        m_pDXHandler->dDeviceContext2D.DrawRoundedRect(
            left,         m_GameHeight - (bottom + height),
            left + width, m_GameHeight - bottom,
            radiusX,radiusY,
            lineThickness
        );
    }

    // Strings
    void Engine::DrawString(const tstring& textToDisplay, const Point2f& leftBottom, float width, float height, bool showRect) const
    {
        DrawString(textToDisplay, leftBottom.x, leftBottom.y, width, height, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, const Rectf& destRect, bool showRect) const
    {
        DrawString(textToDisplay, destRect.left, destRect.bottom, destRect.width, destRect.height, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, float left, float bottom, float width, float height, bool showRect) const
    {
        const auto [l, t, r, b] = D2D1::RectF(
            left,
            m_GameHeight - (bottom + height),
            left + width,
            m_GameHeight - bottom);

        if (showRect) m_pDXHandler->dDeviceContext2D.DrawRectangle(l, t, r, b);

        m_pDXHandler->dDeviceContext2D.DrawString(
            to_wstring(textToDisplay),
            l, t, r, b,
            m_pResourceManager->GetCurrentTextFormat()->GetTextFormat()
            );
    }

    void Engine::DrawString(const tstring& textToDisplay, const Point2f& leftBottom, float width, bool showRect) const
    {
       DrawString(textToDisplay, leftBottom.x, leftBottom.y, width, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, float left, float bottom, float width, bool showRect) const
    {
        DrawString(textToDisplay, left, bottom, width, m_pResourceManager->GetCurrentTextFormat()->GetFontSize(), showRect);
    }

    //Textures
    void Engine::DrawTexture(const Texture* texture, float destLeft, float destBottom, const Rectf& srcRect, float opacity) const
    {
        DrawTexture(texture, Rectf{ destLeft, destBottom, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void Engine::DrawTexture(const Texture* texture, const Point2f& destLeftBottom, const Rectf& srcRect, float opacity) const
    {
        DrawTexture(texture, Rectf{ destLeftBottom.x, destLeftBottom.y, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void Engine::DrawTexture(const Texture* texture, const Rectf& destRect, const Rectf& srcRect, float opacity) const
    {
        const Vector2f& wndwSize = GetGameSize();

        const D2D1_RECT_F destination = D2D1::RectF(
            destRect.left,
            wndwSize.y - (destRect.bottom + destRect.height),
            destRect.left + destRect.width,
            wndwSize.y - destRect.bottom
        );

        if (!texture)
        {
            m_pDXHandler->dDeviceContext2D.FillRectangle(destination.left, destination.top, destination.right, destination.bottom);
            OutputDebugString(_T("ERROR! Texture was nullptr in DrawTexture!\n"));
            return;
        }

        D2D1_RECT_F source{};
        if ((srcRect.width <= 0 || srcRect.height <= 0) && texture)
        {
            source = D2D1::RectF(0.F,0.F,texture->GetWidth(),texture->GetHeight());
        }
        else
        {
            constexpr float sliceMargin{ 0.07f };
            source = D2D1::RectF(
                srcRect.left + sliceMargin,
                srcRect.bottom + sliceMargin,
                srcRect.left + srcRect.width - sliceMargin,
                srcRect.bottom + srcRect.height - sliceMargin);
        }

        m_pDXHandler->dDeviceContext2D.DrawTexture(
            texture->GetBitmap(),
            destination, source,
            opacity
        );
    }


    //-----------------
    //Fill
    //----------------

    //Rectangles
    void Engine::FillRectangle(const Point2f& leftBottom, float width, float height) const
    {
        FillRectangle(leftBottom.x, leftBottom.y, width, height);
    }
    void Engine::FillRectangle(const Rectf& rect) const
    {
        FillRectangle(rect.left, rect.bottom, rect.width, rect.height);
    }
    void Engine::FillRectangle(float left, float bottom, float width, float height) const
    {
        m_pDXHandler->dDeviceContext2D.FillRectangle(
            left,         m_GameHeight - (bottom + height),
            left + width, m_GameHeight - bottom
            );
    }

    //RoundedRects
    void Engine::FillRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY) const
    {
        m_pDXHandler->dDeviceContext2D.FillRoundedRect(
            left,         m_GameHeight - (bottom + height),
            left + width, m_GameHeight - bottom,
            radiusX, radiusY
            );
    }
    void Engine::FillRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY) const
    {
        FillRoundedRect(leftBottom.x, leftBottom.y, width, height, radiusX, radiusY);
    }
    void Engine::FillRoundedRect(const Rectf& rect, float radiusX, float radiusY) const
    {
        FillRoundedRect(rect.left, rect.bottom, rect.width, rect.height, radiusX, radiusY);
    }

#else

    //Rectangles

    void Engine::DrawRectangle(const Point2f& leftTop, float width, float height, float lineThickness) const
    {
        DrawRectangle(leftTop.x, leftTop.y, width, height, lineThickness);
    }
    void Engine::DrawRectangle(const Rectf& rect, float lineThickness) const
    {
        DrawRectangle(rect.left, rect.top, rect.width, rect.height, lineThickness);
    }
    void Engine::DrawRectangle(float left, float top, float width, float height, float lineThickness) const
    {
        m_pDXHandler->dDeviceContext2D.DrawRectangle(left, top, left + width, top + height, lineThickness);
    }

    //RoundedRects
    void Engine::DrawRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY, float lineThickness) const
    {
        DrawRoundedRect(leftTop.x, leftTop.y, width, height, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness) const
    {
        DrawRoundedRect(rect.left, rect.top, rect.width, rect.height, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY, float lineThickness) const
    {
        m_pDXHandler->dDeviceContext2D.DrawRoundedRect(
            left, top, left + width, top + height,
            radiusX,radiusY,
            lineThickness
        );
    }

    //String
    void Engine::DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, float height, bool showRect) const
    {
        DrawString(textToDisplay, leftTop.x, leftTop.y, width, height, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, const Rectf& destRect, bool showRect) const
    {
        DrawString(textToDisplay, destRect.left, destRect.top, destRect.width, destRect.height, showRect);
    }
    void Engine::DrawString(const tstring& textToDisplay, float left, float top, float width, float height, bool showRect) const
    {
        auto [l,t,r,b] = D2D1::RectF(left, top, left + width, top + height);

        if (showRect) m_pDXHandler->dDeviceContext2D.DrawRectangle(l, t, r, b);

        m_pDXHandler->dDeviceContext2D.DrawString(
            to_wstring(textToDisplay),
            l,t,r,b,
            m_pResourceManager->GetCurrentTextFormat()->GetTextFormat()
            );
    }

    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void Engine::DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, bool showRect) const
    {
        DrawString(textToDisplay, leftTop.x, leftTop.y, width, showRect);
    }
    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void Engine::DrawString(const tstring& textToDisplay, float left, float top, float width, bool showRect) const
    {
        DrawString(textToDisplay, left, top, width, m_pResourceManager->GetCurrentTextFormat()->GetFontSize(), showRect);
    }


    //Textures
    void Engine::DrawTexture(const Texture* texture, float destLeft, float destTop, const Rectf& srcRect, float opacity) const
    {
        DrawTexture(texture, Rectf{ destLeft, destTop, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void Engine::DrawTexture(const Texture* texture, const Point2f& destLeftTop, const Rectf& srcRect, float opacity) const
    {
        DrawTexture(texture, Rectf{ destLeftTop.x, destLeftTop.y, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void Engine::DrawTexture(const Texture* texture, const Rectf& destRect, const Rectf& srcRect, float opacity) const
    {
        const D2D1_RECT_F destination = D2D1::RectF(destRect.left, destRect.top, destRect.left + destRect.width, destRect.top + destRect.height);

        if (!texture)
        {
            m_pDXHandler->dDeviceContext2D.FillRectangle(destination.left, destination.top, destination.right, destination.bottom);
            OutputDebugString(_T("ERROR! Texture was nullptr in DrawTexture!\n"));
        }
        else
        {

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
            m_pDXHandler->dDeviceContext2D.DrawTexture(texture->GetBitmap(), destination, source, opacity);
        }
    }

    //-----------------
    //Fill
    //-----------------

    //Rectangles
    void Engine::FillRectangle(const Point2f& leftTop, float width, float height) const
    {
        FillRectangle(leftTop.x, leftTop.y, width, height);
    }
    void Engine::FillRectangle(const Rectf& rect) const
    {
        FillRectangle(rect.left, rect.top, rect.width, rect.height);
    }
    void Engine::FillRectangle(float left, float top, float width, float height) const
    {
        m_pDXHandler->dDeviceContext2D.FillRectangle(left, top, left + width, top + height);
    }

    //RoundedRects
    void Engine::FillRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY) const
    {
        FillRoundedRect(leftTop.x, leftTop.y, width, height, radiusX, radiusY);
    }
    void Engine::FillRoundedRect(const Rectf& rect, float radiusX, float radiusY) const
    {
        FillRoundedRect( rect.left, rect.top, rect.width, rect.height, radiusX, radiusY);
    }
    void Engine::FillRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY) const
    {
        m_pDXHandler->dDeviceContext2D.FillRoundedRect(
            left, top, left + width, top + height,
            radiusX, radiusY
            );
    }

#endif // MATHEMATICAL_COORDINATESYSTEM

    //Ellipse
    void Engine::DrawEllipse(const Point2f& center, float radiusX, float radiusY, float lineThickness) const
    {
        DrawEllipse(center.x, center.y, radiusX, radiusY, lineThickness);
    }
    void Engine::DrawEllipse(const Ellipsef& ellipse, float lineThickness) const
    {
        DrawEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY, lineThickness);
    }
    void Engine::DrawCircle(const Circlef& circle, float lineThickness) const
    {
        DrawEllipse(circle.center, circle.rad, circle.rad, lineThickness);
    }
    void Engine::DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineThickness) const
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM) centerY = m_GameHeight - centerY;

        m_pDXHandler->dDeviceContext2D.DrawEllipse(
            centerX, centerY,
            radiusX, radiusY,
            lineThickness
        );
    }

    void Engine::FillEllipse(const Point2f& center, float radiusX, float radiusY) const
    {
        FillEllipse(center.x, center.y, radiusX, radiusY);
    }
    void Engine::FillEllipse(const Ellipsef& ellipse) const
    {
        FillEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY);
    }
    void Engine::FillCircle(const Circlef& circle) const
    {
        FillEllipse(circle.center, circle.rad, circle.rad);
    }
    void Engine::FillEllipse(float centerX, float centerY, float radiusX, float radiusY) const
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM) centerY = m_GameHeight - centerY;

        m_pDXHandler->dDeviceContext2D.FillEllipse(
            centerX, centerY,
            radiusX, radiusY
            );
    }

    //Geometry
    void Engine::DrawPolygon(const Polygon& polygon, float lineThickness) const
    {
        DrawGeometry(&polygon, lineThickness);
    }

    void Engine::FillPolygon(const Polygon& polygon) const
    {
        FillGeometry(&polygon);
    }

    void Engine::DrawArc(const Arc& arc, float lineThickness) const
    {
        DrawGeometry(&arc, lineThickness);
    }

    void Engine::FillArc(const Arc& arc) const
    {
        FillGeometry(&arc);
    }

    void Engine::DrawGeometry(const Geometry* const pGeometryObject, float lineThickness) const
    {
        PushTransform();
        Translate(pGeometryObject->GetTranslation());
        m_pDXHandler->dDeviceContext2D.DrawGeometry(pGeometryObject->GetGeometry(), lineThickness);
        PopTransform();
    }
    void Engine::FillGeometry(const Geometry* const pGeometryObject) const
    {
        PushTransform();
        Translate(pGeometryObject->GetTranslation());
        m_pDXHandler->dDeviceContext2D.FillGeometry(pGeometryObject->GetGeometry());
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
        SetWindowPosition(refreshWindowPos, true);
    }
    void Engine::SetWindowScale(float scale)
    {
        m_WindowScale = scale;
    }
    void Engine::SetWindowPosition(bool setPos, bool setSize)
    {
        MONITORINFOEX mi{};
        mi.cbSize = sizeof(MONITORINFOEX);

        if (GetMonitorInfo(MonitorFromWindow(m_hWindow, MONITOR_DEFAULTTOPRIMARY), &mi))
        {
            ::SetWindowLongPtr(m_hWindow, GWL_STYLE, m_OriginalStyle);

            const UINT dpi = GetDpiForWindow(m_hWindow);
            const float dpiMultiplier = dpi / static_cast<float>(USER_DEFAULT_SCREEN_DPI);

            if (setSize)
            {
                m_WindowWidth = static_cast<int>(m_GameWidth * m_WindowScale * dpiMultiplier);
                m_WindowHeight = static_cast<int>(m_GameHeight * m_WindowScale * dpiMultiplier);
            }

            const int windowWidth{ static_cast<int>(GetSystemMetricsForDpi(SM_CXFIXEDFRAME, dpi) * 2 + m_WindowWidth + m_WindowPosOffset * dpiMultiplier * 2) };
            const int windowHeight{ static_cast<int>(GetSystemMetricsForDpi(SM_CYFIXEDFRAME, dpi) * 2 +
                                GetSystemMetricsForDpi(SM_CYCAPTION, dpi) + m_WindowHeight + m_WindowPosOffset * dpiMultiplier * 2) };

            if (setPos)
            {
                m_WindowPosX = mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left) / 2 - windowWidth / 2;
                m_WindowPosY = mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2 - windowHeight / 2;
            }

            ::SetWindowPos(m_hWindow, nullptr, m_WindowPosX, m_WindowPosY, windowWidth, windowHeight, SWP_FRAMECHANGED);

            m_IsFullscreen = false;
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

            m_WindowPosX = mi.rcMonitor.left;
            m_WindowPosY = mi.rcMonitor.top;

            ::SetWindowPos(m_hWindow, nullptr, m_WindowPosX, m_WindowPosY, m_WindowWidth, m_WindowHeight, SWP_FRAMECHANGED);
            m_IsFullscreen = true;
        }
        if (m_pGame)
        {
            ShowWindow(m_hWindow, SW_DENORMAL);
            UpdateWindow(m_hWindow);
        }
    }
    void Engine::CalculateWindowPos()
    {
        LPRECT lpRect{ new RECT{} };
        ::GetWindowRect(m_hWindow, lpRect);
        m_WindowPosX = lpRect->left;
        m_WindowPosY = lpRect->top;

        delete lpRect;
    }
    HResultHandler Engine::ResizeWindow() const
    {
        OutputDebugString(_T("ResizeWindow\n"));
        HResultHandler hr{S_OK, _T("ENGINE::ResizeWindow")};
        hr = m_pDXHandler->ResizeWindow();
        return hr;
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

    void Engine::Translate(float xTranslation, float yTranslation) const
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM) yTranslation *= -1;
        m_pDXHandler->dDeviceContext2D.Translate(xTranslation, yTranslation);
    }

    void Engine::Rotate(float angle, float xPivotPoint, float yPivotPoint) const
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM) yPivotPoint = m_GameHeight - yPivotPoint;
        m_pDXHandler->dDeviceContext2D.Rotate(angle, xPivotPoint, yPivotPoint);
    }
    void Engine::Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom) const
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM) yPointToScaleFrom = m_GameHeight - yPointToScaleFrom;
        m_pDXHandler->dDeviceContext2D.Scale(xScale, yScale, xPointToScaleFrom, yPointToScaleFrom);
    }

    void Engine::PushTransform() const
    {
        m_pDXHandler->dDeviceContext2D.PushTransform();
    }

    void Engine::PopTransform() const
    {
        m_pDXHandler->dDeviceContext2D.PopTransform();
    }

    void Engine::Translate(const Vector2f& translation) const
    {
        Translate(translation.x, translation.y);
    }
    void Engine::Rotate(float angle, const Point2f& pivotPoint) const
    {
        Rotate(angle, pivotPoint.x, pivotPoint.y);
    }
    void Engine::Scale(float scale, float xPointToScaleFrom, float yPointToScaleFrom) const
    {
        Scale(scale, scale, xPointToScaleFrom, yPointToScaleFrom);
    }
    void Engine::Scale(float xScale, float yScale, const Point2f& PointToScaleFrom) const
    {
        Scale(xScale, yScale, PointToScaleFrom.x, PointToScaleFrom.y);
    }
    void Engine::Scale(float scale, const Point2f& PointToScaleFrom) const
    {
        Scale(scale, scale, PointToScaleFrom.x, PointToScaleFrom.y);
    }
    void Engine::Scale(float xScale, float yScale) const
    {
        Scale(xScale, yScale, 0, 0);
    }
    void Engine::Scale(float scale) const
    {
        Scale(scale, 0, 0);
    }

    void Engine::AddController()
    {
        if (m_pVecControllers.size() < 4)
        {
            m_pVecControllers.emplace_back(std::make_unique<Controller>(static_cast<uint8_t>(m_pVecControllers.size())));
        }
        else OutputDebugString(_T( "Max amount of controllers already reached.\n"));
    }

    void Engine::PopController()
    {
        if (not m_pVecControllers.empty()) m_pVecControllers.pop_back();
    }

    void Engine::PopAllControllers()
    {
        m_pVecControllers.clear();
    }

    bool Engine::IsAnyControllerButtonPressed() const
    {
        return std::ranges::any_of(m_pVecControllers, [](const auto& pController)
        {
            return pController->IsAnyButtonPressed();
        });
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
    void Engine::VibrateController(int strengthPercentage, uint8_t controllerIndex) const
    {
        if (controllerIndex < m_pVecControllers.size())
        {
            m_pVecControllers.at(controllerIndex)->Vibrate(strengthPercentage);
        }
        else
            OutputDebugString(std::format(_T("Trying to vibrate controller, but controller for controllerIndex {} not found.\n"), controllerIndex).c_str());
    }
    Vector2f Engine::GetControllerJoystickValue(bool leftJoystick, uint8_t controllerIndex) const
    {
        return m_pVecControllers.at(controllerIndex)->GetJoystickValue(leftJoystick);
    }
    float Engine::GetControllerTriggerValue(bool leftTrigger, uint8_t controllerIndex) const
    {
        return m_pVecControllers.at(controllerIndex)->GetTriggerValue(leftTrigger);
    }
    void Engine::SetJoystickDeadzone(bool left, int percentage, uint8_t controllerIndex)
    {
        return m_pVecControllers.at(controllerIndex)->SetJoystickDeadzone(left, percentage);
    }
    void Engine::SetTriggerDeadzone(bool left, int percentage, uint8_t controllerIndex)
    {
        return m_pVecControllers.at(controllerIndex)->SetTriggerDeadzone(left, percentage);
    }


    void Engine::ShowMouse(bool show) const
    {
        ShowCursor(show);

        InvalidateRect(m_hWindow, nullptr, true);
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

    void Engine::SetColor(COLORREF newColor, float opacity) const
    {
        m_pDXHandler->dDeviceContext2D.SetDrawColor(newColor, opacity);
    }
    void Engine::SetBackGroundColor(COLORREF newColor, float opacity) const
    {
        m_pDXHandler->dDeviceContext2D.SetBackgroundColor(newColor, opacity);
    }

    void Engine::Paint() const
    {
        OnRender();
        ValidateRect(m_hWindow, nullptr);
    }

    ResourceManager* Engine::ResourceMngr() const
    {
        return m_pResourceManager.get();
    }
    SceneManager* Engine::SceneMngr() const
    {
        return m_pSceneManager.get();
    }

    const Font* Engine::GetCurrentFont() const
    {
        return m_pResourceManager->GetCurrentFont();
    }

    Vector2f Engine::GetGameSize() const
    {
        return { static_cast<float>(m_GameWidth), static_cast<float>(m_GameHeight) };
    }
    Point2f Engine::GetViewportPos() const
    {
        return Point2f{
            static_cast<float>(m_ViewPortTranslationX),
            static_cast<float>(m_ViewPortTranslationY)
        };
    }
    Vector2f Engine::GetViewportSize() const
    {
        return Vector2f{
            static_cast<float>(m_ViewPortWidth),
            static_cast<float>(m_ViewPortHeight)
        };
    }
    Point2f Engine::GetWindowPos() const
    {
        return Point2f{
            static_cast<float>(m_WindowPosX),
            static_cast<float>(m_WindowPosY)
        };
    }
    Vector2f Engine::GetWindowSize() const
    {
        return Vector2f{
            static_cast<float>(m_WindowWidth),
            static_cast<float>(m_WindowHeight)
        };
    }
    float Engine::GetWindowScale() const
    {
        return m_WindowScale;
    }
    HWND Engine::GetWindow() const
    {
        return m_hWindow;
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
    bool Engine::IsQuitting() const
    {
        return m_IsQuitting;
    }
    const DX::Factory2D& Engine::Get2DFactory() const
    {
        return m_pDXHandler->dFactory2D;
    }
    const DX::DeviceContext2D& Engine::Get2DDeviceContext() const
    {
        return m_pDXHandler->dDeviceContext2D;
    }
}
