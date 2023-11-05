#pragma once

#include "resource.h"
//#include "framework.h"
#include "windows.h"

class Engine
{
private:
    Engine() = default;
    static Engine* m_pEngine;
public:

    Engine(const Engine& other) = delete;
    Engine(Engine&& other) noexcept = delete;
    Engine& operator=(const Engine& other) = delete;
    Engine& operator=(Engine&& other) noexcept = delete;

    ~Engine() = default;

    static Engine* GetSingleton();


    void SetColor(COLORREF newColor);
    void PaintLine(POINT first, POINT second);
    int Run(HINSTANCE& hInst);

private:
    COLORREF m_PaintColor;
};