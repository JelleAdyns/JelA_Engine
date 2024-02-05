#ifndef ENGINE_H
#define ENGINE_H

#include "resource.h"
#include "framework.h"
#include "BaseGame.h"
#include "Structs.h"

class Engine final
{
private:
    Engine();
    static Engine* m_pEngine;
public:

    Engine(const Engine& other) = delete;
    Engine(Engine&& other) noexcept = delete;
    Engine& operator=(const Engine& other) = delete;
    Engine& operator=(Engine&& other) noexcept = delete;

    ~Engine();

    static Engine* GetSingleton();

    int Run(int nCmdShow);
    LRESULT HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


    void DrawLine           (int firstX, int firstY, int secondX, int secondY, float lineThickness = 1.f) const;
    void DrawLine           (const Point2Int& firstPoint, const Point2Int& secondPoint, float lineThickness = 1.f) const;

#ifdef MATHEMATICAL_COORDINATESYSTEM
    void DrawRectangle      (int left, int bottom, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const Point2Int& leftBottom, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const RectInt& rect, float lineThickness = 1.f)const;
    void DrawRoundedRect    (int left, int bottom, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const RectInt& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;

    void FillRectangle(int left, int bottom, int width, int height)const;
    void FillRectangle(const Point2Int& leftBottom, int width, int height)const;
    void FillRectangle(const RectInt& rect)const;
    void FillRoundedRect(int left, int bottom, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect(const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect(const RectInt& rect, float radiusX, float radiusY)const;
#else
    void DrawRectangle      (int left, int top, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const Point2Int& leftTop, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const RectInt& rect, float lineThickness = 1.f)const;
    void DrawRoundedRect    (int left, int top, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const Point2Int& leftTop, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const RectInt& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;

    void FillRectangle(int left, int top, int width, int height)const;
    void FillRectangle(const Point2Int& leftTop, int width, int height)const;
    void FillRectangle(const RectInt& rect)const;
    void FillRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect(const Point2Int& leftTop, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect(const RectInt& rect, float radiusX, float radiusY)const;
#endif // MATHEMATICAL_COORDINATESYSTEM

    void DrawEllipse        (int centerX, int centerY, int radiusX, int radiusY, float lineThickness = 1.f)const;
    void DrawEllipse        (const Point2Int& center, int radiusX, int radiusY, float lineThickness = 1.f)const;
    void DrawEllipse        (const EllipseInt& ellipse, float lineThickness = 1.f)const;

    void FillEllipse(int centerX, int centerY, int radiusX, int radiusY)const;
    void FillEllipse(const Point2Int& center, int radiusX, int radiusY)const;
    void FillEllipse(const EllipseInt& ellipse)const;

    void DrawString         (int left, int top, int width, int height)const;
    void DrawBitmap         (int left, int top, int width, int height)const;


    void SetColor(COLORREF newColor, float opacity = 1.F);
    void SetBackGroundColor(COLORREF newColor);
    void SetInstance(HINSTANCE hInst);
    void SetTitle(const tstring& newTitle);
    void SetWindowDimensions(int width, int height);
    void SetFrameRate(int FPS);

    RectInt GetWindowSize() const;
    
    static ID2D1HwndRenderTarget*  m_pDRenderTarget;
private:
    void DrawBorders(int rtWidth, int rtHeight, FLOAT translationX, FLOAT translationY) const;
    RectInt GetRenderTargetSize() const;
    HRESULT MakeWindow(int nCmdShow);
    HRESULT CreateOurRenderTarget();


    HWND m_hWindow;
    HINSTANCE m_hInstance;

    ID2D1Factory* m_pDFactory;
    ID2D1SolidColorBrush* m_pDColorBrush;
    D2D1_COLOR_F m_DColorBackGround;

    BaseGame* m_pGame;



    tstring* m_pTitle;
    int m_Width;
    int m_Height;

    double m_TimePerFrame;
};

class Texture final
{
public:
    Texture(const tstring& filename);

    Texture(const Texture& other) = delete;
    Texture(Texture&& other) noexcept = delete;
    Texture& operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) noexcept = delete;

    void Draw()
    {

            //Create a Direct2D bitmap from the WIC bitmap.
        if (!m_pDBitmap)
        {
            Engine::m_pDRenderTarget->CreateBitmapFromWicBitmap(
                m_pWICConverter,
                NULL,
                &m_pDBitmap
            );
        }
           

        Engine::m_pDRenderTarget->DrawBitmap(m_pDBitmap,
            D2D1::RectF(0.f, 0.f, m_pDBitmap->GetSize().width, m_pDBitmap->GetSize().height), 1.f,D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    }

    ~Texture() 
    { 
        SafeRelease(&m_pDBitmap);
        SafeRelease(&m_pWICConverter);
    };
private:
    static IWICImagingFactory* m_pWICFactory;
    IWICFormatConverter* m_pWICConverter;
    ID2D1Bitmap* m_pDBitmap;
};

#endif // !ENGINE_H