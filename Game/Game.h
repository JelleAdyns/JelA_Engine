#ifndef GAME_H
#define GAME_H

#include "Engine.h"
#include "BaseGame.h"

class Game final: public jela::BaseGame
{
public:
    Game() = default;
    virtual ~Game() = default;

    Game(const Game& other) = delete;
    Game(Game&& other) noexcept = delete;
    Game& operator=(const Game& other) = delete;
    Game& operator=(Game&& other) noexcept = delete;

    virtual void Initialize() override;
    virtual void Cleanup() override;
    virtual void Draw() const override;
    virtual void Tick() override;
    virtual void KeyDown(int virtualKeycode) override;
    virtual void KeyDownThisFrame(int virtualKeycode) override;
    virtual void KeyUp(int virtualKeycode) override;
    virtual void HandleControllerInput() override;
    virtual void MouseDown(bool isLeft, float x, float y) override;
    virtual void MouseUp(bool isLeft, float x, float y) override;
    virtual void MouseMove(float x, float y, int keyDown) override;
    virtual void MouseWheelTurn(float x, float y, int turnDistance, int keyDown) override;
private:

};

#endif // !GAME_H