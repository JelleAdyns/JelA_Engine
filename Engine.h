#pragma once

#include "resource.h"
//#include "framework.h"
#include <windows.h>
#include "Application.h"
#include "Defines.h"

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

    ~Engine() = default;

    static Engine* GetSingleton();


    void SetColor(COLORREF newColor);
    void PaintLine(POINT first, POINT second, HDC hDc);
    void PaintLine(POINT first, POINT second);

    int Run(int nCmdShow);
    bool MakeWindow(int nCmdShow);
    HINSTANCE GetInstance() const;
    void SetInstance(HINSTANCE hInst);
    void SetTitle(const tstring& newTitle);
    void SetWindow(HWND hWindow);
    void SetWindowDimensions(POINT windowDimensions);
    LRESULT HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void SetFrameRate(int framesPerSecond) { m_SecondsBetweenFrames = 1.f / framesPerSecond; }
private:
    COLORREF m_PaintColor;
    Application* m_pGame;
    HDC m_PaintHdc;
    HINSTANCE m_hInstance;
    HWND m_hWindow;

    tstring* m_pTitle;
    int m_Width;
    int m_Height;

    float m_SecondsBetweenFrames;
};
