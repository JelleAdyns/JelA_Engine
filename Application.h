#pragma once
class Application
{
public:
    Application() = default;
    ~Application() = default;

    Application(const Application& other) = delete;
    Application(Application&& other) noexcept = delete;
    Application& operator=(const Application& other) = delete;
    Application& operator=(Application&& other) noexcept = delete;

	void Paint() const;
	void Tick();
private:


};

