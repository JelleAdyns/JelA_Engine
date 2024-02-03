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
    void DrawRectangle(int left, int top, int width, int height, float lineThickness = 1.f)const;
    void DrawRoundedRect(int left, int top, int width, int height, float radius, float lineThickness = 1.f)const;
    void DrawRoundedRect(int left, int top, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawEllipse(int centerX, int centerY, int radius, float lineThickness = 1.f)const;
    void DrawEllipse(int centerX, int centerY, int radiusX, int radiusY, float lineThickness = 1.f)const;
    void DrawString(int left, int top, int width, int height)const;
    void DrawBitmap(int left, int top, int width, int height)const;
    void FillRectangle(int left, int top, int width, int height)const;

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