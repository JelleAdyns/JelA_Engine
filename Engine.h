#ifndef ENGINE_H
#define ENGINE_H

#include "resource.h"
#include "framework.h"
#include "Application.h"

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

    void SetColor(COLORREF newColor, FLOAT opacity = 1.F);
    //void PaintLine(POINT first, POINT second, HDC hDc);
    void PaintLine(int firstX, int firstY, int secondX, int secondY, FLOAT lineThickness = 1.f);

   // HINSTANCE GetInstance() const;
    void SetInstance(HINSTANCE hInst);
    void SetTitle(const tstring& newTitle);
    //void SetWindow(HWND hWindow);
    void SetWindowDimensions(POINT windowDimensions);

private:

    HRESULT MakeWindow(int nCmdShow);
    HRESULT CreateOurRenderTarget();

    HWND m_hWindow;
    HINSTANCE m_hInstance;
    ID2D1Factory* m_pDFactory;
    ID2D1HwndRenderTarget* m_pDRenderTarget;

    Application* m_pGame;

    ID2D1SolidColorBrush* m_pDColorBrush;
    COLORREF m_PaintColor;
    HDC m_PaintHdc;

    tstring* m_pTitle;
    int m_Width;
    int m_Height;
};

#endif // !ENGINE_H