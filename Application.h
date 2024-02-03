#ifndef APPLICATION_H
#define APPLICATION_H


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
	void Draw();
	void Tick();

    int variable{};
private:
    
    float m_X{};
    int width{ 700 };
    int height{ 500 };
};

#endif // !APPLICATION_H