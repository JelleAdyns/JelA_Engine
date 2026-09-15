#ifndef BASEGAME_H
#define BASEGAME_H

namespace jela
{
    namespace mouse
    {
        enum class ModButtons;
    }

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
        virtual void MouseDown(mouse::ModButtons, float, float, mouse::ModButtons) {}
        virtual void MouseUp(mouse::ModButtons, float, float, mouse::ModButtons) {}
        virtual void MouseDoubleClick(mouse::ModButtons, float, float, mouse::ModButtons) {}
        virtual void MouseMove(float, float, mouse::ModButtons) {}
        virtual void MouseWheelTurn(float, float, int, mouse::ModButtons) {}
    };

}
#endif // !BASEGAME_H