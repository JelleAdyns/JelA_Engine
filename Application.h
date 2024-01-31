#pragma once
#include <windows.h>
class Application
{
public:
    Application() = default;
    ~Application() = default;

    Application(const Application& other) = delete;
    Application(Application&& other) noexcept = delete;
    Application& operator=(const Application& other) = delete;
    Application& operator=(Application&& other) noexcept = delete;

	void Initialize();
	void Paint();
	void Tick();

    int variable{};
private:
    POINT m_point{};
    float m_X{};
};

