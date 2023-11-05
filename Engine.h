#pragma once

#include "resource.h"


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

    static Engine* GetSingleton();

    int Run(HINSTANCE& hInst);
};