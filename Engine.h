#ifndef ENGINE_H
#define ENGINE_H

#include "resource.h"
#include "framework.h"
#include "Application.h"
#include "Structs.h"

class Engine
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

    //void PaintLine(POINT first, POINT second, HDC hDc);
    void DrawLine(int firstX, int firstY, int secondX, int secondY, float lineThickness = 1.f) const;
    void DrawLine(const Point2Int& firstPoint, const Point2Int& secondPoint, float lineThickness = 1.f) const;

#ifdef MATHEMATICAL_COORDINATESYSTEM
    void DrawRectangle(int left, int bottom, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle(const Point2Int& leftBottom, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle(const RectInt& rect, float lineThickness = 1.f)const;
    void DrawRoundedRect(int left, int bottom, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect(const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect(const RectInt& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;
#else
    void DrawRectangle(int left, int top, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle(const Point2Int& leftTop, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle(const RectInt& rect, float lineThickness = 1.f)const;
    void DrawRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect(const Point2Int& leftTop, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect(const RectInt& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;
#endif // MATHEMATICAL_COORDINATESYSTEM

    void DrawEllipse(int centerX, int centerY, int radiusX, int radiusY, float lineThickness = 1.f)const;
    void DrawEllipse(const Point2Int& center, int radiusX, int radiusY, float lineThickness = 1.f)const;
    void DrawEllipse(const EllipseInt& ellipse, float lineThickness = 1.f)const;

    void DrawString(int left, int top, int width, int height)const;
    void DrawBitmap(int left, int top, int width, int height)const;

#ifdef MATHEMATICAL_COORDINATESYSTEM
    void FillRectangle(int left, int bottom, int width, int height)const;
    void FillRectangle(const Point2Int& leftBottom, int width, int height)const;
    void FillRectangle(const RectInt& rect)const;
    void FillRoundedRect(int left, int bottom, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect(const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect(const RectInt& rect, float radiusX, float radiusY)const;
#else
    void FillRectangle(int left, int top, int width, int height)const;
    void FillRectangle(const Point2Int& leftTop, int width, int height)const;
    void FillRectangle(const RectInt& rect)const;
    void FillRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect(const Point2Int& leftTop, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect(const RectInt& rect, float radiusX, float radiusY)const;
#endif // MATHEMATICAL_COORDINATESYSTEM

    void FillEllipse(int centerX, int centerY, int radiusX, int radiusY)const;
    void FillEllipse(const Point2Int& center, int radiusX, int radiusY)const;
    void FillEllipse(const EllipseInt& ellipse)const;

    void SetColor(COLORREF newColor, float opacity = 1.F);
    void SetInstance(HINSTANCE hInst);
    void SetTitle(const tstring& newTitle);
    void SetWindowDimensions(int width, int height);
    void SetFrameRate(int FPS);

    RectInt GetWindowSize() const;
    
private:
    void DrawBorders(int rtWidth, int rtHeight, FLOAT translationX, FLOAT translationY) const;
    RectInt GetRenderTargetSize() const;
    HRESULT MakeWindow(int nCmdShow);
    HRESULT CreateOurRenderTarget();


    HWND m_hWindow;
    HINSTANCE m_hInstance;

    ID2D1Factory* m_pDFactory;
    ID2D1HwndRenderTarget* m_pDRenderTarget;
    ID2D1SolidColorBrush* m_pDColorBrush;

    Application* m_pGame;



    tstring* m_pTitle;
    int m_Width;
    int m_Height;

    double m_TimePerFrame;
};

#endif // !ENGINE_H