#ifndef BASEGAME_H
#define BASEGAME_H

namespace jela
{
    class BaseGame
    {
    public:
        BaseGame() = default;
        virtual ~BaseGame() = default;

        BaseGame(const BaseGame& other) = delete;
        BaseGame(BaseGame&& other) noexcept = delete;
        BaseGame& operator=(const BaseGame& other) = delete;
        BaseGame& operator=(BaseGame&& other) noexcept = delete;

        virtual void Initialize() {};
        virtual void Cleanup() {};

        virtual void Draw() const {}
        virtual void Tick() {}
        virtual void KeyDown(int) {}
        virtual void KeyDownThisFrame(int) {}
        virtual void KeyUp(int) {}
        virtual void HandleControllerInput() {}
        virtual void MouseDown(bool, int, int) {}
        virtual void MouseUp(bool, int, int) {}
        virtual void MouseMove(int, int, int) {}
        virtual void MouseWheelTurn(int, int, int, int) {}
    };

}
#endif // !BASEGAME_H