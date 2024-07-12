#ifndef GAME_H
#define GAME_H

#include "Engine.h"

class Game final: public BaseGame
{
public:
    Game();
    virtual ~Game();

    Game(const Game& other) = delete;
    Game(Game&& other) noexcept = delete;
    Game& operator=(const Game& other) = delete;
    Game& operator=(Game&& other) noexcept = delete;

    virtual void Initialize() override;
    virtual void Draw() const override;
    virtual void Tick(float elapsedSec) override;
    virtual void KeyDown(int virtualKeycode) override;
    virtual void KeyUp(int virtualKeycode) override;
    virtual void MouseDown(bool isLeft, int x, int y) override;
    virtual void MouseUp(bool isLeft, int x, int y) override;
    virtual void MouseMove(int x, int y, int keyDown) override;
    virtual void MouseWheelTurn(int x, int y, int turnDistance, int keyDown) override;
private:

    float m_X{};
    float m_Y{};
    int width{ 1000 };
    int height{ 800 };
    float velocity{ 0 };
    float maxVelocity{ 100 };
    float acceleration{ -98.f };
    float angle{};
    float spritePos{};
    int spritevelocity{};


    enum class Clips
    {
        Spaceship,
        Saw,
        EnemyPopped
    };

    Texture* m_Texture{ new Texture{ L"Wesley.png"} };
    Font* m_pFont{ new Font{L"castlevania-3.ttf", 60, false, false, true} };
    Font* m_pFont2{ new Font{L"DIN-Light.otf", 60, false, false, true}};
    //Audio* M_pAudio{ new Audio{L"Resources\\Spaceship.mp3", false} };
    //Audio* M_pAudio2{ new Audio{L"C:\\Users\\jelle\\Documents\\DAE-GD\\Semester-3\\Programming-3\\Castlevania_III\\Resources\\saw_sfx1.mp3", true} };
    //Audio* M_pAudio3{ new Audio{L"C:\\Users\\jelle\\Documents\\DAE-GD\\Semester-4\\Programming-4\\Prog4Engine\\Data\\Sounds\\enemyPopped.wav", true} };
};

#endif // !GAME_H