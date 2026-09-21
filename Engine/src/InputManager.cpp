#include "InputManager.h"
#include "Engine.h"

namespace jela
{
    bool ContainsMouseButtons(mouse::ModButtons collectionOfButtons,mouse::ModButtons buttonsToCheckFor)
    {
        const auto buttonsToCheckValue = static_cast<int>(buttonsToCheckFor);
        return (static_cast<int>(collectionOfButtons) & buttonsToCheckValue) == buttonsToCheckValue;
    }

    void InputManager::ProcessInput()
    {
        if (!m_AreCommandsActive) return;

        for (auto& pController : m_Controllers)
        {
            if (pController.IsAnyButtonPressed()) ExecuteControllerCommands();
        }

        while (!m_EventQueue.empty())
        {
            ExecuteKeyBoardCommands();

            const auto event = m_EventQueue.front();
            m_CurrProcessedEvent = event;

            for (const auto & [state, pCommand] : m_KeyCommands)
            {
                const auto & [key, keyState] = state;
                if (keyState == keys::State::UpThisFrame) { if (KeyUpThisFrame(event, key)) pCommand->Execute(); }
                else if (keyState == keys::State::DownThisFrame) { if (KeyDownThisFrame(event, key)) pCommand->Execute(); }
                else if (keyState == keys::State::Down) { if (KeyDown(event, key)) pCommand->Execute();}
            }
            for (const auto & [state, pCommand] : m_MouseCommands)
            {
                const auto & [button, mouseState] = state;
                if (mouseState == mouse::State::UpThisFrame) { if (MouseUpThisFrame(event, button)) pCommand->Execute(); }
                else if (mouseState == mouse::State::DownThisFrame) { if (MouseDownThisFrame(event, button)) pCommand->Execute(); }
                else if (mouseState == mouse::State::DoubleClicked) { if (MouseDoubleClicked(event, button)) pCommand->Execute(); }
                else if (mouseState == mouse::State::Scrolled) { if (MouseScrolled(event, button)) pCommand->Execute(); }
                else if (mouseState == mouse::State::Moved) { if (MouseMoved(event, button)) pCommand->Execute(); }
            }

            m_EventQueue.pop();
        }
        for (const auto & [state, pCommand] : m_KeyCommands)
        {
            const auto & [key, keyState] = state;
            if (keyState == keys::State::Pressed) { if (KeyPressed(key)) pCommand->Execute(); }
            else if (keyState == keys::State::NotPressed) { if (!KeyPressed(key)) pCommand->Execute(); }
        }
        for (const auto & [state, pCommand] : m_MouseCommands)
        {
            const auto & [button, mouseState] = state;
            if (mouseState == mouse::State::Pressed) { if (MousePressed(button)) pCommand->Execute(); }
            else if (mouseState == mouse::State::NotPressed) { if (!MousePressed(button)) pCommand->Execute(); }
        }

        for (auto& controller : m_Controllers) controller.ProcessControllerInput();

    }
    void InputManager::QueueEvent(EventInfo&& event)
    {
        m_EventQueue.emplace(event);
    }
    bool InputManager::KeyPressed(keys::VirtualKey virtualKeycode)
    {
        return GetKeyState(virtualKeycode) < 0 && ENGINE.Window()->IsWindowActive();
    }
    bool InputManager::MouseDownThisFrame(const EventInfo& event, mouse::Actions action)
    {
        if (action == mouse::Actions::Left)  return event.event == WindowEvent::MouseLeftDown;
        if (action == mouse::Actions::Right) return event.event == WindowEvent::MouseRightDown;
        if (action == mouse::Actions::Middle) return event.event == WindowEvent::MouseMiddleDown;
        return false;
    }
    bool InputManager::MouseUpThisFrame(const EventInfo& event, mouse::Actions action)
    {
        if (action == mouse::Actions::Left) return event.event == WindowEvent::MouseLeftUp;
        if (action == mouse::Actions::Right) return event.event == WindowEvent::MouseRightUp;
        if (action == mouse::Actions::Middle) return event.event == WindowEvent::MouseMiddleUp;
        return false;
    }
    bool InputManager::MouseDoubleClicked(const EventInfo& event, mouse::Actions action)
    {
        if (action == mouse::Actions::Left) return event.event == WindowEvent::MouseLeftDouble;
        if (action == mouse::Actions::Right) return event.event == WindowEvent::MouseRightDouble;
        if (action == mouse::Actions::Middle) return event.event == WindowEvent::MouseMiddleDouble;
        return false;
    }
    bool InputManager::MouseScrolled(const EventInfo& event, mouse::Actions action)
    {
        return event.event == WindowEvent::MouseWheel && action == mouse::Actions::Scroll;
    }
    bool InputManager::MouseMoved(const EventInfo& event, mouse::Actions action)
    {
        return event.event == WindowEvent::MouseMove && action == mouse::Actions::Move;
    }
    bool InputManager::MousePressed(mouse::Actions action)
    {
        if (action == mouse::Actions::Left) return KeyPressed(VK_LBUTTON);
        if (action == mouse::Actions::Right) return KeyPressed(VK_RBUTTON);
        if (action == mouse::Actions::Middle) return KeyPressed(VK_MBUTTON);
        return false;
    }
    bool InputManager::ButtonDownThisFrame(Controller::Button button, Controller::ID controllerIndex)
    {
        return m_Controllers.at(controllerIndex).IsDownThisFrame(button);
    }
    bool InputManager::ButtonUpThisFrame(Controller::Button button, Controller::ID controllerIndex)
    {
        return m_Controllers.at(controllerIndex).IsUpThisFrame(button);
    }
    bool InputManager::ButtonPressed(Controller::Button button, Controller::ID controllerIndex)
    {
        return m_Controllers.at(controllerIndex).IsPressed(button);
    }
    void InputManager::AddKeyCommand(const std::shared_ptr<Command>& pCommand, keys::VirtualKey virtualKeycode, keys::State keyState)
    {
        const KeyBoardState state{virtualKeycode, keyState};
#ifndef NDEBUG
        if (m_KeyCommands.contains(state))
            OutputDebugString( std::format(_T("Binding to the requested key ({}) already exists. Overwriting now.\n"),virtualKeycode).c_str());
#endif // !NDEBUG
        m_KeyCommands[state] = pCommand;
    }
    void InputManager::AddMouseCommand(const std::shared_ptr<Command>& pCommand, mouse::Actions mouseButton, mouse::State mouseState)
    {
        const MouseState state{mouseButton, mouseState};
#ifndef NDEBUG
        if (m_MouseCommands.contains(state))
            OutputDebugString( std::format(_T("Binding to the requested mouse action ({}) already exists. Overwriting now.\n"), static_cast<int>(mouseButton)).c_str());
#endif // !NDEBUG
        m_MouseCommands[state] = pCommand;
    }
    void InputManager::AddControllerCommand(const std::shared_ptr<Command>& pCommand, Controller::Button button, Controller::ButtonState buttonState, Controller::ID controllerIndex)
    {
        m_Controllers.at(controllerIndex).AddCommand(pCommand, button, buttonState);
    }
    void InputManager::AddCommand_ChangingToController(std::unique_ptr<Command>&& pCommand)
    {
        m_pChangingToControllerCommands.emplace_back(std::move(pCommand));
    }
    void InputManager::AddCommand_ChangingToKeyboard(std::unique_ptr<Command>&& pCommand)
    {
        m_pChangingToKeyboardCommands.emplace_back(std::move(pCommand));
    }
    void InputManager::AddController()
    {
        if (m_Controllers.size() < 4)
        {
            m_Controllers.emplace_back(static_cast<Controller::ID>( m_Controllers.size()));
        }
#ifndef NDEBUG
        else std::cout << "Max amount of controllers already reached.\n";
#endif // !NDEBUG
    }
    void InputManager::RemoveAllCommands()
    {
        m_KeyCommands.clear();
        m_MouseCommands.clear();

        for (auto& pController : m_Controllers)
            pController.RemoveAllCommands();

    }
    void InputManager::RemoveKeyCommand(keys::VirtualKey virtualKeycode, keys::State keyState)
    {
        if (const KeyBoardState state{virtualKeycode, keyState}; m_KeyCommands.contains(state))
            m_KeyCommands.erase(state);
    }
    void InputManager::RemoveMouseCommand(mouse::Actions mouseButton, mouse::State mouseState)
    {
        if (const MouseState state{mouseButton, mouseState}; m_MouseCommands.contains(state))
            m_MouseCommands.erase(state);
    }
    void InputManager::RemoveControllerCommand(Controller::Button button, Controller::ButtonState buttonState, Controller::ID controllerIndex)
    {
        m_Controllers.at(controllerIndex).RemoveCommand(button, buttonState);
    }
    void InputManager::RemoveChangingToKeyboardCommands()
    {
        m_pChangingToKeyboardCommands.clear();
    }
    void InputManager::RemoveChangingToControllerCommands()
    {
        m_pChangingToControllerCommands.clear();
    }
    void InputManager::PopController()
    {
        if (!m_Controllers.empty()) m_Controllers.pop_back();
    }
    void InputManager::PopAllControllers()
    {
        m_Controllers.clear();
    }
    void InputManager::DeactivateAllCommands()
    {
        m_AreCommandsActive = false;
    }
    void InputManager::ActivateAllCommands()
    {
        m_AreCommandsActive = true;
    }
    void InputManager::VibrateController(uint8_t strengthPercentage, Controller::ID controllerIndex)
    {
        m_Controllers.at(controllerIndex).Vibrate(strengthPercentage);
    }
    Vector2f InputManager::GetJoystickValue(bool leftJoystick, Controller::ID controllerIndex)
    {
        return m_Controllers.at(controllerIndex).GetJoystickValue(leftJoystick);
    }
    float InputManager::GetTriggerValue(bool leftTrigger, Controller::ID controllerIndex)
    {
        return m_Controllers.at(controllerIndex).GetTriggerValue(leftTrigger);
    }
    int InputManager::AmountOfControllersConnected()
    {
        return Controller::AmountOfConnectedControllers();
    }
    Point2f InputManager::GetCurrentMousePosition() const
    {
        if (m_CurrProcessedEvent.event == WindowEvent::MouseWheel)
            return CalculateWheelMousePosition(m_CurrProcessedEvent.lParam);
        return CalculateMousePosition(m_CurrProcessedEvent.lParam);
    }
    int InputManager::GetCurrentMouseWheelDelta() const
    {
        if (m_CurrProcessedEvent.event != WindowEvent::MouseWheel)
            throw std::runtime_error{"Currently Processed input event was not a mouse wheel event when trying to get its Delta value!"};
        return GET_WHEEL_DELTA_WPARAM(m_CurrProcessedEvent.wParam);
    }
    bool InputManager::KeyDownThisFrame(const EventInfo& event, keys::VirtualKey virtualKeycode)
    {
        return event.event == WindowEvent::KeyDown && GetKeyRepeatCount(event.lParam) == 0 && GetKey(event.wParam) == virtualKeycode;
    }
    bool InputManager::KeyUpThisFrame(const EventInfo& event, keys::VirtualKey virtualKeycode)
    {
        return event.event == WindowEvent::KeyUp && GetKey(event.wParam) == virtualKeycode;
    }
    bool InputManager::KeyDown(const EventInfo& event, keys::VirtualKey virtualKeycode)
    {
        return event.event == WindowEvent::KeyDown && GetKey(event.wParam) == virtualKeycode;
    }
    Point2f InputManager::CalculateMousePosition(LPARAM lParam)
    {
        const auto& wnd = ENGINE.Window();
        const auto& viewportPos = wnd->GetViewportPos();
        const auto& viewportSize = wnd->GetViewportSize();
        const auto& gameSize = wnd->GetGameSize();

        // X-coordinate
        float xCoordinate = (GET_X_LPARAM(lParam) - viewportPos.x) / viewportSize.x * gameSize.x;
        xCoordinate = std::round(xCoordinate);

        // Y-coordinate
        float yCoordinate = (GET_Y_LPARAM(lParam) - viewportPos.y) / viewportSize.y * gameSize.y;
        if (USE_MATHEMATICAL_COORDINATESYSTEM) yCoordinate = gameSize.y - yCoordinate;
        yCoordinate = std::round(yCoordinate);

        return Point2f{xCoordinate, yCoordinate};
    }
    Point2f InputManager::CalculateWheelMousePosition(LPARAM lParam)
    {
        const auto& wnd = ENGINE.Window();
        const auto& viewportPos = wnd->GetViewportPos();
        const auto& viewportSize = wnd->GetViewportSize();
        const auto& gameSize = wnd->GetGameSize();
        const auto& windowPos = wnd->GetWindowPos();

        const UINT dpi = GetDpiForWindow(wnd->GetWindow());

        // X-Coordinate
        float screenPosX = (GET_X_LPARAM(lParam) - windowPos.x - viewportPos.x);
        if (!wnd->IsFullscreen()) screenPosX -= GetSystemMetricsForDpi(SM_CXFIXEDFRAME, dpi) + wnd->GetWindowPosOffset();

        float xWheelCoordinate = screenPosX / viewportSize.x * gameSize.x;
        xWheelCoordinate = std::round(xWheelCoordinate);

        // Y-Coordinate
        float screenPosY = (GET_Y_LPARAM(lParam) - windowPos.y - viewportPos.y);
        if (!wnd->IsFullscreen()) screenPosY -= GetSystemMetricsForDpi(SM_CXFIXEDFRAME, dpi) + wnd->GetWindowPosOffset() + GetSystemMetricsForDpi(SM_CYCAPTION, dpi);

        float yWheelCoordinate = screenPosY / viewportSize.y * gameSize.y;

        if (USE_MATHEMATICAL_COORDINATESYSTEM) yWheelCoordinate = gameSize.y - yWheelCoordinate;

        yWheelCoordinate = std::round(yWheelCoordinate);

        return Point2f{xWheelCoordinate, yWheelCoordinate};
    }
    keys::VirtualKey InputManager::GetKey(WPARAM wParam)
    {
        return static_cast<keys::VirtualKey>(wParam);
    }
    int InputManager::GetKeyRepeatCount(LPARAM lParam)
    {
        return (lParam & (1 << 30));
    }
    void InputManager::ExecuteKeyBoardCommands()
    {
        m_IsKeyboardActive = true;
        for (const auto& pCommand : m_pChangingToKeyboardCommands)
            pCommand->Execute();
    }
    void InputManager::ExecuteControllerCommands()
    {
        m_IsKeyboardActive = false;
        for (const auto& pCommand : m_pChangingToControllerCommands)
            pCommand->Execute();
    }
    bool InputManager::KeyBoardState::operator==(const KeyBoardState& other) const
    {
        return key == other.key && keyState == other.keyState;
    }
    std::size_t InputManager::KeyBoardStateHasher::operator()(const KeyBoardState& keyboardState) const
    {
        constexpr std::size_t nrOfAvailableBytes = sizeof(std::size_t);
        constexpr std::size_t bytesPerMember = (nrOfAvailableBytes > 1) ? (nrOfAvailableBytes / 2) : 1;

        auto result = static_cast<std::size_t>(keyboardState.keyState) << bytesPerMember * 8;
        result |= static_cast<std::size_t>(keyboardState.key);

        return result;
    }
    bool InputManager::MouseState::operator==(const MouseState& other) const
    {
        return action == other.action && mouseState == other.mouseState;
    }
    std::size_t InputManager::MouseStateHasher::operator()(const MouseState& mouseState) const
    {
        auto result = static_cast<std::size_t>(mouseState.mouseState) << 8;
        result |= static_cast<std::size_t>(mouseState.action);

        return result;
    }
} // jela