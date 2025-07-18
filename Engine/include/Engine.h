#ifndef ENGINE_H
#define ENGINE_H

#include "BaseGame.h"
#include "Structs.h"
#include "Geometry.h"
#include "CPlayer.h"
#include "Audio.h"
#include "framework.h"
#include "Controller.h"
#include "Observer.h"
#include "ResourceManager.h"
#include <vector>
#include <chrono>


namespace jela
{

    class Engine final
    {
    public:
        Engine();
        Engine(const Engine& other) = delete;
        Engine(Engine&& other) noexcept = delete;
        Engine& operator=(const Engine& other) = delete;
        Engine& operator=(Engine&& other) noexcept = delete;

        ~Engine() = default;

        void Shutdown();
        bool Init(HINSTANCE hInstance, const tstring& resourcePath, int width, int height, const COLORREF& bgColor, const tstring& wndwName);
      

        int Run(std::unique_ptr<BaseGame>&& game);
        LRESULT HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        void Quit();

        void DrawLine(const Point2f& firstPoint, const Point2f& secondPoint, float lineThickness = 1.f) const;
        void DrawLine(float firstX, float firstY, const Point2f& secondPoint, float lineThickness = 1.f) const;
        void DrawLine(const Point2f& firstPoint, float secondX, float secondY, float lineThickness = 1.f) const;
        void DrawLine(float firstX, float firstY, float secondX, float secondY, float lineThickness = 1.f) const;

        void DrawVector(const Point2f& origin, const Vector2f& vector, float headLineLength = 30.f, float lineThickness = 1.f) const;
        void DrawVector(const Point2f& origin, float vectorX, float vectorY, float headLineLength = 30.f, float lineThickness = 1.f) const;
        void DrawVector(float originX, float originY, const Vector2f& vector, float headLineLength = 30.f, float lineThickness = 1.f) const;
        void DrawVector(float originX, float originY, float vectorX, float vectorY, float headLineLength = 30.f, float lineThickness = 1.f) const;

#ifdef MATHEMATICAL_COORDINATESYSTEM
        void DrawRectangle(const Point2f& leftBottom, float width, float height, float lineThickness = 1.f)const;
        void DrawRectangle(const Rectf& rect, float lineThickness = 1.f)const;
        void DrawRectangle(float left, float bottom, float width, float height, float lineThickness = 1.f)const;

        void DrawRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY, float lineThickness = 1.f)const;

        void DrawString(const tstring& textToDisplay, const Point2f& leftBottom, float width, float height, bool showRect = false)const;
        void DrawString(const tstring& textToDisplay, const Rectf& destRect, bool showRect = false)const;
        void DrawString(const tstring& textToDisplay, float left, float bottom, float width, float height, bool showRect = false)const;

        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, const Point2f& leftBottom, float width, bool showRect = false)const;
        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, float left, float bottom, float width, bool showRect = false)const;

        void DrawTexture(const Texture* const texture, float destLeft, float destBottom, const Rectf& srcRect = {}, float opacity = 1.f)const;
        void DrawTexture(const Texture* const texture, const Point2f& destLeftBottom = {}, const Rectf& srcRect = {}, float opacity = 1.f)const;
        void DrawTexture(const Texture* const texture, const Rectf& destRect, const Rectf& srcRect = {}, float opacity = 1.f)const;

        void FillRectangle(const Point2f& leftBottom, float width, float height)const;
        void FillRectangle(const Rectf& rect)const;
        void FillRectangle(float left, float bottom, float width, float height)const;

        void FillRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY)const;
        void FillRoundedRect(const Rectf& rect, float radiusX, float radiusY)const;
        void FillRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY)const;
#else
        void DrawRectangle(const Point2f& leftTop, float width, float height, float lineThickness = 1.f)const;
        void DrawRectangle(const Rectf& rect, float lineThickness = 1.f)const;
        void DrawRectangle(float left, float top, float width, float height, float lineThickness = 1.f)const;

        void DrawRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY, float lineThickness = 1.f)const;

        void DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, float height, bool showRect = false)const;
        void DrawString(const tstring& textToDisplay, const Rectf& destRect, bool showRect = false)const;
        void DrawString(const tstring& textToDisplay, float left, float top, float width, float height, bool showRect = false)const;

        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, bool showRect = false)const;
        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, float left, float top, float width, bool showRect = false)const;

        void DrawTexture(const Texture* const texture, float destLeft, float destTop, const Rectf& srcRect = {}, float opacity = 1.f)const;
        void DrawTexture(const Texture* const texture, const Point2f& destLeftTop = {}, const Rectf& srcRect = {}, float opacity = 1.f)const;
        void DrawTexture(const Texture* const texture, const Rectf& destRect, const Rectf& srcRect = {}, float opacity = 1.f)const;

        void FillRectangle(const Point2f& leftTop, float width, float height)const;
        void FillRectangle(const Rectf& rect)const;
        void FillRectangle(float left, float top, float width, float height)const;

        void FillRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY)const;
        void FillRoundedRect(const Rectf& rect, float radiusX, float radiusY)const;
        void FillRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY)const;
#endif // MATHEMATICAL_COORDINATESYSTEM

        void DrawPolygon(const Polygon& polygon, float lineThickness = 1.f);
        void FillPolygon(const Polygon& polygon);

        void DrawArc(const Arc& arc, float lineThickness = 1.f);
        void FillArc(const Arc& arc);

        void DrawEllipse(const Point2f& center, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawEllipse(const Ellipsef& ellipse, float lineThickness = 1.f)const;
        void DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawCircle(const Circlef& circle, float lineThickness = 1.f)const;

        void FillEllipse(const Point2f& center, float radiusX, float radiusY)const;
        void FillEllipse(const Ellipsef& ellipse)const;
        void FillEllipse(float centerX, float centerY, float radiusX, float radiusY)const;
        void FillCircle(const Circlef& circle)const;

        //Use CAPITAL letters or the virtual keycodes
        bool IsKeyPressed(int virtualKeycode) const;

        // Controller stuff

        void AddController();
        void PopController();
        void PopAllControllers();
        bool IsAnyButtonPressed() const;
        bool ButtonDownThisFrame(Controller::Button button, uint8_t controllerIndex) const;
        bool ButtonUpThisFrame(Controller::Button button, uint8_t controllerIndex) const;
        bool ButtonPressed(Controller::Button button, uint8_t controllerIndex)  const;

        // Transform stuff

        void PushTransform();
        void PopTransform();
        void Translate(float xTranslation, float yTranslation);
        void Translate(const Vector2f& translation);
        void Rotate(float angle, float xPivotPoint, float yPivotPoint);
        void Rotate(float angle, const Point2f& pivotPoint);
        void Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom);
        void Scale(float scale, float xPointToScaleFrom, float yPointToScaleFrom);
        void Scale(float xScale, float yScale, const Point2f& PointToScaleFrom);
        void Scale(float scale, const Point2f& PointToScaleFrom);
        void Scale(float xScale, float yScale);
        void Scale(float scale);


        // Setters

        void ShowMouse(bool show);
        void SetVSync(bool enable);
        void SetFont(const Font* const pFont);
        void SetTextFormat(TextFormat* const pTextFormat);
        void SetColor(COLORREF newColor, float opacity = 1.F);
        void SetBackGroundColor(COLORREF newColor);
        void SetInstance(HINSTANCE hInst);
        void SetTitle(const tstring& newTitle);
        void SetWindowDimensions(int width, int height, bool refreshWindowPos = true);
        void SetWindowScale(float scale);
        void SetFrameRate(int FPS);

        // Getters

        ResourceManager* const ResourceMngr() const;
        const Font* const GetCurrentFont() const;
        Rectf GetWindowRect() const;
        float GetWindowScale() const;
        HWND GetWindow() const;
        HINSTANCE GetHInstance() const;
        float GetDeltaTime() const;
        float GetTotalTime() const;
        bool IsKeyBoardActive() const;

        ID2D1Factory* GetFactory() const;
        ID2D1HwndRenderTarget* GetRenderTarget() const;
        ID2D1BitmapRenderTarget* GetBitmapRenderTarget() const;


        static void NotifyError(HWND hWnd, const tstring& pszErrorMessage, HRESULT hrErr)
        {
            const size_t MESSAGE_LEN = 512;
            TCHAR message[MESSAGE_LEN];

            if (SUCCEEDED(StringCchPrintf(message, MESSAGE_LEN, _T("%s (HRESULT = 0x%X)"),
                pszErrorMessage.c_str(), hrErr)))
            {
                MessageBox(hWnd, message, _T("ERROR"), MB_OK | MB_ICONERROR);
            }
        }

      
    private:

        void DrawGeometry(const Geometry* const pGeometryObject, float lineThickness = 1.f);
        void FillGeometry(const Geometry* const pGeometryObject);
        void SetWindowPosition();
        void SetFullscreen();
        void SetTransform() const;
        void SetDeltaTime(float elapsedSec);
        Rectf GetRenderTargetSize() const;
        void Paint();
        HRESULT OnRender();
        HRESULT MakeWindow();
        HRESULT CreateRenderTargets();
        void ResetRenderTargets();

        //Win32
        HWND                            m_hWindow;
        HINSTANCE                       m_hInstance;
        DWORD                           m_OriginalStyle;
        LARGE_INTEGER                   m_TriggerCount{};

        //Direct2D
        ID2D1Factory*                   m_pDFactory{};
        ID2D1HwndRenderTarget*          m_pDRenderTarget{};
        ID2D1SolidColorBrush*           m_pDColorBrush{};
        D2D1_COLOR_F                    m_DColorBackGround{};
        ID2D1BitmapRenderTarget*        m_pDBitmapRenderTarget{};
        ID2D1Bitmap*                    m_pDBitmap{};

        //BaseGame
        std::unique_ptr<BaseGame>       m_pGame{};

        //Transform
        FLOAT                           m_ViewPortTranslationX{};
        FLOAT                           m_ViewPortTranslationY{};

        std::vector<D2D1::Matrix3x2F>   m_VecTransformMatrices{};

        mutable bool                    m_TransformChanged{};

        //General datamembers
        tstring                         m_Title{};

        float                           m_WindowScale{ 1 };
        int                             m_GameWidth{};
        int                             m_GameHeight{};
        int                             m_WindowWidth{};
        int                             m_WindowHeight{};

        float                           m_SecondsPerFrame{};
        float                           m_DeltaTime{};
        float                           m_TotalTime{};

        bool                            m_IsFullscreen{};
        bool                            m_KeyIsDown{};
        bool                            m_WindowIsActive{ true };
        bool                            m_IsKeyboardActive{true};
        bool                            m_IsVSyncEnabled{true};

        std::vector<std::unique_ptr<Controller>> m_pVecControllers{};

        std::unique_ptr<ResourceManager>m_pResourceManager{};
    };
    //---------------------------------------------------------------



    //---------------------------------------------------------------
    namespace utils
    {
        // Following functions originate from Koen Samyn, professor Game Development at Howest

        float Distance(float x1, float y1, float x2, float y2);
        float Distance(const Point2f& p1, const Point2f& p2);

        bool IsPointInRect(const Point2f& p, const Rectf& r);
        bool IsPointInCircle(const Point2f& p, const Circlef& c);
        bool IsPointInEllipse(const Point2f& p, const Ellipsef& e);

        bool IsOverlapping(const Point2f& a, const Point2f& b, const Circlef& c);
        bool IsOverlapping(const Point2f& a, const Point2f& b, const Ellipsef& e);
        bool IsOverlapping(const Point2f& a, const Point2f& b, const Rectf& r);
        bool IsOverlapping(const Rectf& r1, const Rectf& r2);
        bool IsOverlapping(const Rectf& r, const Circlef& c);
        bool IsOverlapping(const Circlef& c1, const Circlef& c2);

        Point2f ClosestPointOnLine(const Point2f& point, const Point2f& linePointA, const Point2f& linePointB);
        float DistPointLineSegment(const Point2f& point, const Point2f& linePointA, const Point2f& linePointB);
        bool IsPointOnLineSegment(const Point2f& point, const Point2f& linePointA, const Point2f& linePointB);
        bool IntersectLines(const Vector2f& l1, const Vector2f& l2, const Point2f& origin1 = {}, const Point2f& origin2 = {});
        bool IntersectLines(const Point2f& p1, const Point2f& p2, const Point2f& q1, const Point2f& q2);
        bool IntersectLineSegments(const Point2f& p1, const Point2f& p2, const Point2f& q1, const Point2f& q2, float& line1Interpolation, float& line2Interpolation);
        bool IntersectRectLine(const Rectf& r, const Point2f& p1, const Point2f& p2, std::pair<Point2f, Point2f>& intersections);
    }
    //---------------------------------------------------------------
}


// Extern declaration of the Engine global
extern jela::Engine ENGINE;

#endif // !ENGINE_H