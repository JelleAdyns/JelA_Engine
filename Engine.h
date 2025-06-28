#ifndef ENGINE_H
#define ENGINE_H

#include "BaseGame.h"
#include "Structs.h"
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

        void DrawLine(const Point2Int& firstPoint, const Point2Int& secondPoint, float lineThickness = 1.f) const;
        void DrawLine(int firstX, int firstY, const Point2Int& secondPoint, float lineThickness = 1.f) const;
        void DrawLine(const Point2Int& firstPoint, int secondX, int secondY, float lineThickness = 1.f) const;
        void DrawLine(int firstX, int firstY, int secondX, int secondY, float lineThickness = 1.f) const;

        void DrawVector(const Point2Int& origin, const Vector2f& vector, int headLineLength = 30.f, float lineThickness = 1.f) const;
        void DrawVector(const Point2Int& origin, float vectorX, float vectorY, int headLineLength = 30.f, float lineThickness = 1.f) const;
        void DrawVector(int originX, int originY, const Vector2f& vector, int headLineLength = 30.f, float lineThickness = 1.f) const;
        void DrawVector(int originX, int originY, float vectorX, float vectorY, int headLineLength = 30.f, float lineThickness = 1.f) const;

#ifdef MATHEMATICAL_COORDINATESYSTEM
        void DrawRectangle(const Point2Int& leftBottom, int width, int height, float lineThickness = 1.f)const;
        void DrawRectangle(const RectInt& rect, float lineThickness = 1.f)const;
        void DrawRectangle(int left, int bottom, int width, int height, float lineThickness = 1.f)const;

        void DrawRoundedRect(const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawRoundedRect(const RectInt& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawRoundedRect(int left, int bottom, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;

        void DrawString(const tstring& textToDisplay, const Point2Int& leftBottom, int width, int height, bool showRect = false)const;
        void DrawString(const tstring& textToDisplay, const RectInt& destRect, bool showRect = false)const;
        void DrawString(const tstring& textToDisplay, int left, int bottom, int width, int height, bool showRect = false)const;

        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, const Point2Int& leftBottom, int width, bool showRect = false)const;
        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, int left, int bottom, int width, bool showRect = false)const;

        void DrawTexture(const Texture* const texture, int destLeft, int destBottom, const RectInt& srcRect = {}, float opacity = 1.f)const;
        void DrawTexture(const Texture* const texture, const Point2Int& destLeftBottom = {}, const RectInt& srcRect = {}, float opacity = 1.f)const;
        void DrawTexture(const Texture* const texture, const RectInt& destRect, const RectInt& srcRect = {}, float opacity = 1.f)const;

        void FillRectangle(const Point2Int& leftBottom, int width, int height)const;
        void FillRectangle(const RectInt& rect)const;
        void FillRectangle(int left, int bottom, int width, int height)const;

        void FillRoundedRect(const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY)const;
        void FillRoundedRect(const RectInt& rect, float radiusX, float radiusY)const;
        void FillRoundedRect(int left, int bottom, int width, int height, float radiusX, float radiusY)const;
#else
        void DrawRectangle(const Point2Int& leftTop, int width, int height, float lineThickness = 1.f)const;
        void DrawRectangle(const RectInt& rect, float lineThickness = 1.f)const;
        void DrawRectangle(int left, int top, int width, int height, float lineThickness = 1.f)const;

        void DrawRoundedRect(const Point2Int& leftTop, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawRoundedRect(const RectInt& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;

        void DrawString(const tstring& textToDisplay, const Point2Int& leftTop, int width, int height, bool showRect = false)const;
        void DrawString(const tstring& textToDisplay, const RectInt& destRect, bool showRect = false)const;
        void DrawString(const tstring& textToDisplay, int left, int top, int width, int height, bool showRect = false)const;

        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, const Point2Int& leftTop, int width, bool showRect = false)const;
        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, int left, int top, int width, bool showRect = false)const;

        void DrawTexture(const Texture* const texture, int destLeft, int destTop, const RectInt& srcRect = {}, float opacity = 1.f)const;
        void DrawTexture(const Texture* const texture, const Point2Int& destLeftTop = {}, const RectInt& srcRect = {}, float opacity = 1.f)const;
        void DrawTexture(const Texture* const texture, const RectInt& destRect, const RectInt& srcRect = {}, float opacity = 1.f)const;

        void FillRectangle(const Point2Int& leftTop, int width, int height)const;
        void FillRectangle(const RectInt& rect)const;
        void FillRectangle(int left, int top, int width, int height)const;

        void FillRoundedRect(const Point2Int& leftTop, int width, int height, float radiusX, float radiusY)const;
        void FillRoundedRect(const RectInt& rect, float radiusX, float radiusY)const;
        void FillRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY)const;
#endif // MATHEMATICAL_COORDINATESYSTEM

        void DrawPolygon(const std::vector<Point2Int>& points, float lineThickness = 1.f, bool closeSegment = false)const;
        void FillPolygon(const std::vector<Point2Int>& points)const;

        void DrawArc(int centerX, int centerY, float radiusX, float radiusY, float startAngle, float angle, float lineThickness = 1.f, bool closeSegment = false)const;
        void DrawArc(const Point2Int& center, float radiusX, float radiusY, float startAngle, float angle, float lineThickness = 1.f, bool closeSegment = false)const;

        void FillArc(int centerX, int centerY, float radiusX, float radiusY, float startAngle, float angle)const;
        void FillArc(const Point2Int& center, float radiusX, float radiusY, float startAngle, float angle)const;

        void DrawEllipse(const Point2Int& center, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawEllipse(const EllipseInt& ellipse, float lineThickness = 1.f)const;
        void DrawEllipse(int centerX, int centerY, float radiusX, float radiusY, float lineThickness = 1.f)const;
        void DrawCircle(const CircleInt& circle, float lineThickness = 1.f)const;

        void FillEllipse(const Point2Int& center, float radiusX, float radiusY)const;
        void FillEllipse(const EllipseInt& ellipse)const;
        void FillEllipse(int centerX, int centerY, float radiusX, float radiusY)const;
        void FillCircle(const CircleInt& circle)const;

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
        void Translate(int xTranslation, int yTranslation);
        void Translate(const Vector2f& translation);
        void Rotate(float angle, int xPivotPoint, int yPivotPoint);
        void Rotate(float angle, const Point2Int& pivotPoint);
        void Scale(float xScale, float yScale, int xPointToScaleFrom, int yPointToScaleFrom);
        void Scale(float scale, int xPointToScaleFrom, int yPointToScaleFrom);
        void Scale(float xScale, float yScale, const Point2Int& PointToScaleFrom);
        void Scale(float scale, const Point2Int& PointToScaleFrom);


        // Setters

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
        RectInt GetWindowRect() const;
        float GetWindowScale() const;
        HWND GetWindow() const;
        HINSTANCE GetHInstance() const;
        float GetDeltaTime() const;
        float GetTotalTime() const;
        bool IsKeyBoardActive() const;

        ID2D1HwndRenderTarget* GetRenderTarget() const;


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


        void CreatePolygon(ID2D1PathGeometry* pGeo, const std::vector<Point2Int>& points, bool closeSegment) const;
        void CreateArc(ID2D1PathGeometry* pGeo, const Point2Int& center, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment) const;
        void DrawBorders(int rtWidth, int rtHeight) const;
        void SetWindowPosition();
        void SetFullscreen();
        void SetTransform() const;
        void SetDeltaTime(float elapsedSec);
        RectInt GetRenderTargetSize() const;
        void Paint();
        HRESULT OnRender();
        HRESULT MakeWindow();
        HRESULT CreateRenderTarget();

        //Win32
        HWND                            m_hWindow;
        HINSTANCE                       m_hInstance;

        //Direct2D
        ID2D1Factory*                   m_pDFactory{};
        ID2D1HwndRenderTarget*          m_pDRenderTarget{};
        ID2D1SolidColorBrush*           m_pDColorBrush{};
        D2D1_COLOR_F                    m_DColorBackGround{};

        //BaseGame
        std::unique_ptr<BaseGame>       m_pGame{};

        //Transform
        FLOAT                           m_ViewPortTranslationX{};
        FLOAT                           m_ViewPortTranslationY{};
        FLOAT                           m_ViewPortScaling{};

        std::vector<D2D1::Matrix3x2F>   m_VecTransformMatrices{};

        mutable bool                    m_TransformChanged{};


        //General datamembers
        tstring                         m_Title{};

        float                           m_WindowScale{ 1 };
        int                             m_Width{};
        int                             m_Height{};

        float                           m_MilliSecondsPerFrame{};
        float                           m_DeltaTime{};
        float                           m_TotalTime{};

        bool                            m_IsFullscreen{};
        bool                            m_KeyIsDown{};
        bool                            m_WindowIsActive{ true };
        bool                            m_IsKeyboardActive{true};

        std::vector<std::unique_ptr<Controller>> m_pVecControllers{};

        std::chrono::high_resolution_clock::time_point m_T1;

        std::unique_ptr<ResourceManager>m_pResourceManager{};
    };
    //---------------------------------------------------------------



    //---------------------------------------------------------------
    namespace utils
    {
        // Following functions originate from Koen Samyn, professor Game Development at Howest

        float Distance(int x1, int y1, int x2, int y2);
        float Distance(const Point2Int& p1, const Point2Int& p2);

        bool IsPointInRect(const Point2Int& p, const RectInt& r);
        bool IsPointInCircle(const Point2Int& p, const CircleInt& c);
        bool IsPointInEllipse(const Point2Int& p, const EllipseInt& e);

        bool IsOverlapping(const Point2Int& a, const Point2Int& b, const CircleInt& c);
        bool IsOverlapping(const Point2Int& a, const Point2Int& b, const EllipseInt& e);
        bool IsOverlapping(const Point2Int& a, const Point2Int& b, const RectInt& r);
        bool IsOverlapping(const RectInt& r1, const RectInt& r2);
        bool IsOverlapping(const RectInt& r, const CircleInt& c);
        bool IsOverlapping(const CircleInt& c1, const CircleInt& c2);

        Point2Int ClosestPointOnLine(const Point2Int& point, const Point2Int& linePointA, const Point2Int& linePointB);
        float DistPointLineSegment(const Point2Int& point, const Point2Int& linePointA, const Point2Int& linePointB);
        bool IsPointOnLineSegment(const Point2Int& point, const Point2Int& linePointA, const Point2Int& linePointB);
        bool IntersectLineSegments(const Point2Int& p1, const Point2Int& p2, const Point2Int& q1, const Point2Int& q2, float& outLambda1, float& outLambda2);
        bool IntersectRectLine(const RectInt& r, const Point2Int& p1, const Point2Int& p2, std::pair<Point2Int, Point2Int>& intersections);
    }
    //---------------------------------------------------------------
}


// Extern declaration of the Engine global
extern jela::Engine ENGINE;

#endif // !ENGINE_H