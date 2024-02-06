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
private:

    float m_X{};
    float m_Y{};
    int width{ 700 };
    int height{ 500 };
    float velocity{ 0 };
    float maxVelocity{ 100 };
    float acceleration{ -98.f };

    Texture* m_Texture{ new Texture{ _T("C:/Users/jelle/Documents/The Clashing Elements/TheClashingElements-SaviorsOfTheMultiverse-Game/Resources/Wesley.png")} };
};

#endif // !GAME_H