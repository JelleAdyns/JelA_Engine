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
    virtual void Draw() override;
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
    int width{ 700 };
    int height{ 500 };
    float velocity{ 0 };
    float maxVelocity{ 100 };
    float acceleration{ -98.f };

    float spritePos{};
    int spritevelocity{};

    Texture* m_Texture{ new Texture{ _T("C:/Users/jelle/Documents/The Clashing Elements/TheClashingElements-SaviorsOfTheMultiverse-Game/Resources/Wesley.png")} };
    Font* m_pFont{ new Font{L"C:/Users/jelle/Documents/DAE GD/Semester 3/Programming 3/Castlevania_III/Resources/castlevania-3.ttf", 60, false, false, true} };
    Font* m_pFont2{ new Font{L"C:/Users/jelle/Documents/DAE GD/Semester 1/Programming 1/7/1DAE10_07_Adyns_Jelle/ImageAndText/Recources/DIN-Light.otf", 60, false, false, true}};
};

#endif // !GAME_H