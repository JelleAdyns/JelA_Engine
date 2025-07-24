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
    virtual void Tick() override;
    virtual void Draw() const override;
    virtual void KeyDown(int virtualKeycode) override;
    virtual void KeyDownThisFrame(int virtualKeycode) override;
    virtual void KeyUp(int virtualKeycode) override;
    virtual void HandleControllerInput() override;
    virtual void MouseDown(jela::MouseButtons buttonDownThisFrame, float x, float y, jela::MouseButtons allDownButtons) override;
    virtual void MouseUp(jela::MouseButtons buttonUpThisFrame, float x, float y, jela::MouseButtons allDownButtons) override;
    virtual void MouseDoubleClick(jela::MouseButtons buttonUpThisFrame, float x, float y, jela::MouseButtons allDownButtons) override;
    virtual void MouseMove(float x, float y, jela::MouseButtons allDownButtons) override;
    virtual void MouseWheelTurn(float x, float y, int turnDistance, jela::MouseButtons allDownButtons) override;
private:

};

#endif // !GAME_H