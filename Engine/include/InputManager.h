#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <queue>
#include "Structs.h"

namespace jela
{
    enum class MouseButtons
    {
        Left = MK_LBUTTON,
        Right = MK_RBUTTON,
        Shift = MK_SHIFT,
        Control = MK_CONTROL,
        Middle = MK_MBUTTON
    };
    bool ContainsMouseButtons(jela::MouseButtons collectionOfButtons, jela::MouseButtons buttonsToCheckFor);
    enum class InputEvent
    {
        KeyUp = WM_KEYUP,
        KeyDown = WM_KEYDOWN,
        MouseLeftUp = WM_LBUTTONUP,
        MouseMiddleUp = WM_MBUTTONUP,
        MouseRightUp = WM_RBUTTONUP,
        MouseLeftDown = WM_LBUTTONDOWN,
        MouseMiddleDown = WM_MBUTTONDOWN,
        MouseRightDown = WM_RBUTTONDOWN,
        MouseLeftDouble = WM_LBUTTONDBLCLK,
        MouseMiddleDouble = WM_MBUTTONDBLCLK,
        MouseRightDouble = WM_RBUTTONDBLCLK,
        MouseMove = WM_MOUSEMOVE,
        MouseWheel = WM_MOUSEWHEEL,
    };
    struct EventInfo
    {
        InputEvent event{};
        WPARAM wParam{};
        LPARAM lParam{};
    };
    class InputManager
    {

    public:

        bool ProcessInput();
        void QueueEvent(const EventInfo& event);
    private:
        static Point2f CalculateMousePosition(LPARAM lParam);
        static Point2f CalculateWheelMousePosition(LPARAM lParam);
        std::queue<EventInfo> m_EventQueue{};
    };

} // jela

#endif //INPUTMANAGER_H
