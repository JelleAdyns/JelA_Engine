#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <queue>
#include <unordered_map>

#include "Command.h"
#include "Controller.h"
#include "Structs.h"

namespace jela
{
    namespace mouse
    {
        enum class Actions : uint8_t
        {
            Left,
            Right,
            Middle,
            Scroll,
            Move
        };
        enum class ModButtons
        {
            Left = MK_LBUTTON,
            Right = MK_RBUTTON,
            Shift = MK_SHIFT,
            Control = MK_CONTROL,
            Middle = MK_MBUTTON
        };
        enum class State : uint8_t
        {
            UpThisFrame,
            DownThisFrame,
            Pressed,
            NotPressed,
            DoubleClicked,
            Scrolled,
            Moved
        };
        bool ContainsMouseButtons(ModButtons collectionOfButtons, ModButtons buttonsToCheckFor);
    }
    namespace keys
    {
        using VirtualKey = uint16_t;

        enum class State : uint8_t
        {
            UpThisFrame,
            DownThisFrame,
            Down,
            Pressed,
            NotPressed
        };
    }
    enum class WindowEvent
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
        WindowEvent event{};
        WPARAM wParam{};
        LPARAM lParam{};
    };
    class InputManager
    {

    public:
        void ProcessInput();
        void QueueEvent(EventInfo&& event);
        bool IsKeyBoardActive() const { return m_IsKeyboardActive; }


        void AddKeyCommand(const std::shared_ptr<Command>& pCommand, keys::VirtualKey virtualKeycode, keys::State keyState);
        void AddMouseCommand(const std::shared_ptr<Command>& pCommand, mouse::Actions mouseButton, mouse::State mouseState);
        static void AddControllerCommand(const std::shared_ptr<Command>& pCommand, Controller::Button button, Controller::ButtonState buttonState, Controller::ID controllerIndex);
        void AddCommand_ChangingToKeyboard(std::unique_ptr<Command>&& pCommand);
        void AddCommand_ChangingToController(std::unique_ptr<Command>&& pCommand);
        static void AddController();

        void RemoveAllCommands();
        void RemoveKeyCommand(keys::VirtualKey virtualKeycode, keys::State keyState);
        void RemoveMouseCommand(mouse::Actions mouseButton, mouse::State mouseState);
        static void RemoveControllerCommand(Controller::Button button, Controller::ButtonState buttonState, Controller::ID controllerIndex);
        void RemoveChangingToKeyboardCommands();
        void RemoveChangingToControllerCommands();
        static void PopController();
        static void PopAllControllers();

        void DeactivateAllCommands();
        void ActivateAllCommands();

        static void VibrateController(uint8_t strengthPercentage, Controller::ID controllerIndex);

        static Vector2f GetJoystickValue(bool leftJoystick, Controller::ID controllerIndex);
        static float GetTriggerValue(bool leftTrigger, Controller::ID controllerIndex);

        static int AmountOfControllersConnected();

        Point2f GetCurrentMousePosition() const;
        int GetCurrentMouseWheelDelta() const;

    private:
        static bool KeyDownThisFrame(const EventInfo& event, keys::VirtualKey virtualKeycode);
        static bool KeyUpThisFrame(const EventInfo& event, keys::VirtualKey virtualKeycode);
        static bool KeyDown(const EventInfo& event, keys::VirtualKey virtualKeycode);
        static bool KeyPressed(keys::VirtualKey virtualKeycode);

        static bool MouseDownThisFrame(const EventInfo& event, mouse::Actions action);
        static bool MouseUpThisFrame(const EventInfo& event, mouse::Actions action);
        static bool MouseDoubleClicked(const EventInfo& event, mouse::Actions action);
        static bool MouseScrolled(const EventInfo& event, mouse::Actions action);
        static bool MouseMoved(const EventInfo& event, mouse::Actions action);
        static bool MousePressed(mouse::Actions action);

        static bool ButtonDownThisFrame(Controller::Button button, Controller::ID controllerIndex);
        static bool ButtonUpThisFrame(Controller::Button button, Controller::ID controllerIndex);
        static bool ButtonPressed(Controller::Button button, Controller::ID controllerIndex);


        static Point2f CalculateMousePosition(LPARAM lParam);
        static Point2f CalculateWheelMousePosition(LPARAM lParam);
        static keys::VirtualKey GetKey(WPARAM wParam);
        static int GetKeyRepeatCount(LPARAM lParam);

        void ExecuteKeyBoardCommands();
        void ExecuteControllerCommands();

        struct KeyBoardState
        {
            keys::VirtualKey key;
            keys::State keyState;

            bool operator==(const KeyBoardState& other) const;
        };
        struct KeyBoardStateHasher
        {
            std::size_t operator()(const KeyBoardState& keyboardState) const;
        };

        struct MouseState
        {
            mouse::Actions action;
            mouse::State mouseState;

            bool operator==(const MouseState& other) const;
        };
        struct MouseStateHasher
        {
            std::size_t operator()(const MouseState& mouseState) const;
        };

        using SharedCommand = std::shared_ptr<Command>;
        using UniqueCommand = std::unique_ptr<Command>;

        std::unordered_map<KeyBoardState, SharedCommand, KeyBoardStateHasher> m_KeyCommands{};
        std::unordered_map<MouseState, SharedCommand, MouseStateHasher> m_MouseCommands{};

        std::vector<UniqueCommand> m_pChangingToControllerCommands{};
        std::vector<UniqueCommand> m_pChangingToKeyboardCommands{};

        static inline std::vector<Controller> m_Controllers{};

        std::queue<EventInfo> m_EventQueue{};
        EventInfo m_CurrProcessedEvent{};

        bool m_IsKeyboardActive{true};
        bool m_AreCommandsActive{true};
    };

} // jela

#endif //INPUTMANAGER_H
