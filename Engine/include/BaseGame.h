#ifndef BASEGAME_H
#define BASEGAME_H

namespace jela
{
    enum class MouseButtons;
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

        virtual void Tick() {}
        virtual void Draw() const {}
        virtual void KeyDown(int) {}
        virtual void KeyDownThisFrame(int) {}
        virtual void KeyUp(int) {}
        virtual void HandleControllerInput() {}
        virtual void MouseDown(MouseButtons, float, float, MouseButtons) {}
        virtual void MouseUp(MouseButtons, float, float, MouseButtons) {}
        virtual void MouseDoubleClick(MouseButtons, float, float, MouseButtons) {}
        virtual void MouseMove(float, float, MouseButtons) {}
        virtual void MouseWheelTurn(float, float, int, MouseButtons) {}
    };

}
#endif // !BASEGAME_H