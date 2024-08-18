#ifndef ENGINE_H
#define ENGINE_H

#include "resource.h"
#include "BaseGame.h"
#include "Structs.h"
#include "CPlayer.h"
#include "Audio.h"
#include "framework.h"
#include <vector>

class Texture;
class Font;

class Engine final
{
private:
    Engine();
public:

    Engine(const Engine& other) = delete;
    Engine(Engine&& other) noexcept = delete;
    Engine& operator=(const Engine& other) = delete;
    Engine& operator=(Engine&& other) noexcept = delete;

    ~Engine();

    static Engine& GetSingleton();

    int Run();
    LRESULT HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


    void DrawLine           (const Point2Int& firstPoint, const Point2Int& secondPoint, float lineThickness = 1.f) const;
    void DrawLine           (int firstX, int firstY, const Point2Int& secondPoint, float lineThickness = 1.f) const;
    void DrawLine           (const Point2Int& firstPoint, int secondX, int secondY, float lineThickness = 1.f) const;
    void DrawLine           (int firstX, int firstY, int secondX, int secondY, float lineThickness = 1.f) const;

    void DrawVector         (const Point2Int& origin, const Vector2f& vector, float lineThickness = 1.f) const;
    void DrawVector         (const Point2Int& origin, float vectorX, float vectorY, float lineThickness = 1.f) const;
    void DrawVector         (int originX, int originY, const Vector2f& vector, float lineThickness = 1.f) const;
    void DrawVector         (int originX, int originY, float vectorX, float vectorY, float lineThickness = 1.f) const;

#ifdef MATHEMATICAL_COORDINATESYSTEM
    void DrawRectangle      (int left, int bottom, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const Point2Int& leftBottom, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const RectInt& rect, float lineThickness = 1.f)const;

    void DrawRoundedRect    (int left, int bottom, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const RectInt& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;

    void DrawString         (const tstring& textToDisplay, const Font& font, int left, int bottom, int width, int height, bool showRect = false)const;
    void DrawString         (const tstring& textToDisplay, const Font& font, Point2Int leftBottom, int width, int height, bool showRect = false)const;
    void DrawString         (const tstring& textToDisplay, const Font& font, RectInt destRect, bool showRect = false)const;

    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void DrawString         (const tstring& textToDisplay, const Font& font, int left, int bottom, int width, bool showRect = false)const;
    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void DrawString         (const tstring& textToDisplay, const Font& font, Point2Int leftBottom, int width, bool showRect = false)const;

    void DrawTexture        (const Texture& texture, int destLeft, int destBottom, const RectInt& srcRect = {}, float opacity = 1.f)const;
    void DrawTexture        (const Texture& texture, const Point2Int& destLeftBottom = {}, const RectInt& srcRect = {}, float opacity = 1.f)const;
    void DrawTexture        (const Texture& texture, const RectInt& destRect, const RectInt& srcRect = {}, float opacity = 1.f)const;

    void FillRectangle      (int left, int bottom, int width, int height)const;
    void FillRectangle      (const Point2Int& leftBottom, int width, int height)const;
    void FillRectangle      (const RectInt& rect)const;

    void FillRoundedRect    (int left, int bottom, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect    (const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect    (const RectInt& rect, float radiusX, float radiusY)const;
#else
    void DrawRectangle      (int left, int top, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const Point2Int& leftTop, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const RectInt& rect, float lineThickness = 1.f)const;

    void DrawRoundedRect    (int left, int top, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const Point2Int& leftTop, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const RectInt& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;

    void DrawString         (const tstring& textToDisplay, const Font& font, int left, int top, int width, int height, bool showRect = false)const;
    void DrawString         (const tstring& textToDisplay, const Font& font, Point2Int leftTop, int width, int height, bool showRect = false)const;
    void DrawString         (const tstring& textToDisplay, const Font& font, RectInt destRect, bool showRect = false)const;

    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void DrawString         (const tstring& textToDisplay, const Font& font, int left, int top, int width, bool showRect = false)const;
    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void DrawString         (const tstring& textToDisplay, const Font& font, Point2Int leftTop, int width, bool showRect = false)const;

    void DrawTexture        (const Texture& texture, int destLeft, int destTop, const RectInt& srcRect = {}, float opacity = 1.f)const;
    void DrawTexture        (const Texture& texture, const Point2Int& destLeftTop = {}, const RectInt& srcRect = {}, float opacity = 1.f)const;
    void DrawTexture        (const Texture& texture, const RectInt& destRect, const RectInt& srcRect = {}, float opacity = 1.f)const;

    void FillRectangle      (int left, int top, int width, int height)const;
    void FillRectangle      (const Point2Int& leftTop, int width, int height)const;
    void FillRectangle      (const RectInt& rect)const;

    void FillRoundedRect    (int left, int top, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect    (const Point2Int& leftTop, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect    (const RectInt& rect, float radiusX, float radiusY)const;
#endif // MATHEMATICAL_COORDINATESYSTEM

    void DrawEllipse        (int centerX, int centerY, int radiusX, int radiusY, float lineThickness = 1.f)const;
    void DrawEllipse        (const Point2Int& center, int radiusX, int radiusY, float lineThickness = 1.f)const;
    void DrawEllipse        (const EllipseInt& ellipse, float lineThickness = 1.f)const;
    void DrawCircle         (const CircleInt& circle, float lineThickness = 1.f)const;

    void FillEllipse        (int centerX, int centerY, int radiusX, int radiusY)const;
    void FillEllipse        (const Point2Int& center, int radiusX, int radiusY)const;
    void FillEllipse        (const EllipseInt& ellipse)const;
    void FillCircle         (const CircleInt& circle)const;

    //Use CAPITAL letters or the virtual keycodes
    bool IsKeyPressed(int virtualKeycode) const;

    void SetColor(COLORREF newColor, float opacity = 1.F);
    void SetBackGroundColor(COLORREF newColor);
    void SetInstance(HINSTANCE hInst);
    void SetTitle(const tstring& newTitle);
    void SetResourcePath(const std::wstring& newTitle);
    void SetWindowDimensions(int width, int height);
    void SetWindowScale(float scale);
    void SetFrameRate(int FPS);

    void PushTransform();
    void PopTransform();
    void Translate(int xTranslation, int yTranslation);
    void Translate(const Vector2f& translation);
    void Rotate(float angle, int xPivotPoint, int yPivotPoint) ;
    void Rotate(float angle, const Point2Int& pivotPoint) ;
    void Scale(float xScale, float yScale, int xPointToScaleFrom, int yPointToScaleFrom);
    void Scale(float scale, int xPointToScaleFrom, int yPointToScaleFrom);
    void Scale(float xScale, float yScale, const Point2Int& PointToScaleFrom);
    void Scale(float scale, const Point2Int& PointToScaleFrom);

    const std::wstring& GetResourcePath() const;
    RectInt GetWindowRect() const;
    float GetWindowScale() const;
    HWND GetWindow() const;
    HINSTANCE GetHInstance() const;
    float GetDeltaTime() const;
    float GetTotalTime() const;

    ID2D1HwndRenderTarget* getRenderTarget() const;


    static void NotifyError(HWND hWnd, const WCHAR* pszErrorMessage, HRESULT hrErr)
    {
        const size_t MESSAGE_LEN = 512;
        WCHAR message[MESSAGE_LEN];

        if (SUCCEEDED(StringCchPrintf(message, MESSAGE_LEN, L"%s (HRESULT = 0x%X)",
            pszErrorMessage, hrErr)))
        {
            MessageBox(hWnd, message, NULL, MB_OK | MB_ICONERROR);
        }
    }

private:

    void DrawBorders(int rtWidth, int rtHeight) const;
    void SetWindowPosition();
    void SetFullscreen();
    void SetTransform() const;
    void SetDeltaTime(float elapsedSec);
    RectInt GetRenderTargetSize() const;
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
    BaseGame*                       m_pGame{};

    //Transform
    FLOAT                           m_ViewPortTranslationX{};
    FLOAT                           m_ViewPortTranslationY{};
    FLOAT                           m_ViewPortScaling{};

    std::vector<D2D1::Matrix3x2F>   m_VecTransformMatrices{};

    bool                            m_TranslationBeforeRotation{};
    mutable bool                    m_TransformChanged{};


    //General datamembers
    std::wstring                    m_ResourcePath{};
    tstring                         m_Title{};

    float                           m_WindowScale{1};
    int                             m_Width{};
    int                             m_Height{};

    float                           m_MilliSecondsPerFrame{};
    float                           m_DeltaTime{};
    float                           m_TotalTime{};

    bool                            m_IsFullscreen{};
    bool                            m_KeyIsDown{};

};


//---------------------------------------------------------------
class Texture final
{
public:
    explicit Texture(const std::wstring& filename);

    Texture(const Texture& other) = delete;
    Texture(Texture&& other) noexcept = delete;
    Texture& operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) noexcept = delete;

    ~Texture();

    ID2D1Bitmap* const  GetBitmap() const { return m_pDBitmap; }
    float GetWidth() const { return m_TextureWidth; }
    float GetHeight() const { return m_TextureHeight; }

private:

    static IWICImagingFactory* m_pWICFactory;
    ID2D1Bitmap* m_pDBitmap;

    float m_TextureWidth;
    float m_TextureHeight;
};
//---------------------------------------------------------------


//---------------------------------------------------------------
//https://stackoverflow.com/questions/37572961/c-directwrite-load-font-from-file-at-runtime
class Font final
{
public:
    Font(const std::wstring& fontname, bool fromFile = false);
    Font(const std::wstring& fontname, int size, bool bold, bool italic, bool fromFile = false);

    Font(const Font& other) = delete;
    Font(Font&& other) noexcept = delete;
    Font& operator=(const Font& other) = delete;
    Font& operator=(Font&& other) noexcept = delete;

    ~Font();

    void SetTextFormat(int size, bool bold, bool italic);
    IDWriteTextFormat* GetFormat() const;
    int GetFontSize() const;

private:
    HRESULT Initialize(const std::wstring& filename);

    static IDWriteFactory5* m_pDWriteFactory;

    IDWriteFontCollection1* m_pFontCollection;
    IDWriteTextFormat* m_pTextFormat;

    tstring m_FontName;
    int m_FontSize;
};
//---------------------------------------------------------------


//---------------------------------------------------------------


template <typename... Args>
class Observer;
template <typename... Args>
class Subject final
{
public:
    Subject() = default;
    ~Subject()
    {
        for (auto& pObserver : m_pVecObservers)
        {
            pObserver->OnSubjectDestroy();
        }
    }

    Subject(const Subject&) = delete;
    Subject(Subject&&) noexcept = delete;
    Subject& operator= (const Subject&) = delete;
    Subject& operator= (Subject&&) noexcept = delete;

    void AddObserver(Observer<Args...>* pObserver)
    {
        if (pObserver)
        {
            auto pos = std::find(m_pVecObservers.cbegin(), m_pVecObservers.cend(), pObserver);
            if (pos == m_pVecObservers.cend())
            {
                m_pVecObservers.push_back(pObserver);
            }
            else throw std::runtime_error("Observer already subscribed to Subject");
        }
    }
    void RemoveObserver(Observer<Args...>* pObserver)
    {
        if (m_pVecObservers.size() > 0)
        {
            auto pos = std::find(m_pVecObservers.cbegin(), m_pVecObservers.cend(), pObserver);
            if (pos != m_pVecObservers.cend()) m_pVecObservers.erase(pos);
#ifndef NDEBUG
            else OutputDebugString(_T("Couldn't find Observer to remove in the vector. Continuing.\n"));
#endif // !NDEBUG
        }
    }

    void NotifyObservers(Args... pSubjectOwner)
    {
        for (auto& pObserver : m_pVecObservers)
        {
            pObserver->Notify(pSubjectOwner);
        }
    }
private:
    std::vector<Observer<Args...>*> m_pVecObservers;

};



template <typename... Args>
class Observer
{
public:

    virtual ~Observer() = default;

    Observer(const Observer&) = delete;
    Observer(Observer&&) noexcept = delete;
    Observer& operator= (const Observer&) = delete;
    Observer& operator= (Observer&&) noexcept = delete;

    virtual void Notify(Args... args) = 0;
    virtual void OnSubjectDestroy() = 0;

protected:
    Observer() = default;
};
//---------------------------------------------------------------




//---------------------------------------------------------------
namespace utils
{
    // Following functions originate from Koen Samyn, professor Game Development at Howest

    float Distance(int x1, int y1, int x2, int y2);
    float Distance(const Point2Int & p1, const Point2Int & p2);

    bool IsPointInRect(const Point2Int & p, const RectInt & r);
    bool IsPointInCircle(const Point2Int & p, const CircleInt & c);
    bool IsPointInEllipse(const Point2Int & p, const EllipseInt & e);

    bool IsOverlapping(const Point2Int & a, const Point2Int & b, const CircleInt & c);
    bool IsOverlapping(const Point2Int & a, const Point2Int & b, const EllipseInt & e);
    bool IsOverlapping(const Point2Int & a, const Point2Int & b, const RectInt & r);
    bool IsOverlapping(const RectInt & r1, const RectInt & r2);
    bool IsOverlapping(const RectInt& r, const CircleInt& c);
    bool IsOverlapping(const CircleInt& c1, const CircleInt& c2);

    Point2Int ClosestPointOnLine(const Point2Int& point, const Point2Int& linePointA, const Point2Int& linePointB);
    float DistPointLineSegment(const Point2Int& point, const Point2Int& linePointA, const Point2Int& linePointB);
    bool IsPointOnLineSegment(const Point2Int& point, const Point2Int& linePointA, const Point2Int& linePointB);
    bool IntersectLineSegments(const Point2Int & p1, const Point2Int & p2, const Point2Int & q1, const Point2Int & q2, float& outLambda1, float& outLambda2);
    bool IntersectRectLine(const RectInt & r, const Point2Int & p1, const Point2Int & p2, std::pair<Point2Int, Point2Int>& intersections);
}
//---------------------------------------------------------------

#endif // !ENGINE_H