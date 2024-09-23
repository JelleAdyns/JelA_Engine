#ifndef BASEGAME_H
#define BASEGAME_H

class BaseGame
{
public:
    BaseGame() = default;
    virtual ~BaseGame() = default;

    BaseGame(const BaseGame& other) = delete;
    BaseGame(BaseGame&& other) noexcept = delete;
    BaseGame& operator=(const BaseGame& other) = delete;
    BaseGame& operator=(BaseGame&& other) noexcept = delete;

    virtual void Initialize();
    virtual void Cleanup();

    virtual void Draw() const {}
	virtual void Tick() {}
    virtual void KeyDown(int virtualKeycode) {}
    virtual void KeyDownThisFrame(int virtualKeycode) {}
    virtual void KeyUp(int virtualKeycode) {}
    virtual void HandleControllerInput() {}
    virtual void MouseDown(bool isLeft, int x, int y) {}
    virtual void MouseUp(bool isLeft, int x, int y) {}
    virtual void MouseMove(int x, int y, int keyDown) {}
    virtual void MouseWheelTurn(int x, int y, int turnDistance, int keyDown) {}
};

#endif // !BASEGAME_H