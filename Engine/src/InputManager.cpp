#include "InputManager.h"
#include "Engine.h"

namespace jela
{
    bool ContainsMouseButtons(jela::MouseButtons collectionOfButtons, jela::MouseButtons buttonsToCheckFor)
    {
        const auto buttonsToCheckValue = static_cast<int>(buttonsToCheckFor);
        return (static_cast<int>(collectionOfButtons) & buttonsToCheckValue) == buttonsToCheckValue;
    }
    bool InputManager::ProcessInput()
    {
        while (!m_EventQueue.empty())
        {
            const auto event = m_EventQueue.front();

            m_EventQueue.pop();
        }

        return true;
    }
    void InputManager::QueueEvent(const EventInfo& event)
    {
        m_EventQueue.push(event);
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
} // jela