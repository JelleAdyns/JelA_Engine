#include "Controller.h"
//#include "GlobalEnumClasses.h"
#include <map>
#include "Windows.h"
#include <XInput.h>

namespace jela
{


	//Implementation
	class Controller::ControllerImpl
	{
	public:
		ControllerImpl(uint8_t controllerIndex) :
			m_ControllerIndex{ controllerIndex },
			m_PreviousState{},
			m_CurrentState{},
			m_ButtonsPressedThisFrame{},
			m_ButtonsReleasedThisFrame{}
		{
			static_assert(static_cast<int>(Button::DpadUp) == XINPUT_GAMEPAD_DPAD_UP);
			static_assert(static_cast<int>(Button::DpadDown) == XINPUT_GAMEPAD_DPAD_DOWN);
			static_assert(static_cast<int>(Button::DpadLeft) == XINPUT_GAMEPAD_DPAD_LEFT);
			static_assert(static_cast<int>(Button::DpadRight) == XINPUT_GAMEPAD_DPAD_RIGHT);
			static_assert(static_cast<int>(Button::Start) == XINPUT_GAMEPAD_START);
			static_assert(static_cast<int>(Button::Back) == XINPUT_GAMEPAD_BACK);
			static_assert(static_cast<int>(Button::LeftThumb) == XINPUT_GAMEPAD_LEFT_THUMB);
			static_assert(static_cast<int>(Button::RightThumb) == XINPUT_GAMEPAD_RIGHT_THUMB);
			static_assert(static_cast<int>(Button::LeftShoulder) == XINPUT_GAMEPAD_LEFT_SHOULDER);
			static_assert(static_cast<int>(Button::RightShoulder) == XINPUT_GAMEPAD_RIGHT_SHOULDER);
			static_assert(static_cast<int>(Button::A) == XINPUT_GAMEPAD_A);
			static_assert(static_cast<int>(Button::B) == XINPUT_GAMEPAD_B);
			static_assert(static_cast<int>(Button::X) == XINPUT_GAMEPAD_X);
			static_assert(static_cast<int>(Button::Y) == XINPUT_GAMEPAD_Y);
		}

		~ControllerImpl() = default;
		ControllerImpl(const ControllerImpl&) = delete;
		ControllerImpl(ControllerImpl&&) noexcept = delete;
		ControllerImpl& operator= (const ControllerImpl&) = delete;
		ControllerImpl& operator= (ControllerImpl&&) noexcept = delete;


		bool IsAnyButtonPressedImpl();
		void ProcessInputImpl();
		bool IsDownThisFrameImpl(Button button) const;
		bool IsUpThisFrameImpl(Button button)  const;
		bool IsPressedImpl(Button button)  const;

		void SetJoystickDeadzone(bool left, int percentage);
		void SetTriggerDeadzone(bool left, int percentage);

		//void AddCommandImpl(const std::shared_ptr<Command>& pCommand, ControllerButton button, KeyState keyState);
		//void RemoveCommandImpl(ControllerButton button, KeyState keyState);
		//void RemoveAllCommandsImpl();

		//void DeactivateAllCommandsImpl();
		//void ActivateAllCommandsImpl();

		void VibrateImpl(int strengthPercentage) const;
		Vector2f GetJoystickValueImpl(bool leftJoystick) const;
		float GetTriggerValueImpl(bool leftJoystick) const;

		static int AmountOfConnectedControllersImpl();

	private:
		uint8_t m_ControllerIndex{};
		XINPUT_STATE m_PreviousState{};
		XINPUT_STATE m_CurrentState{};
		int m_ButtonsPressedThisFrame{};
		int m_ButtonsReleasedThisFrame{};
		int m_LJoystickDeadZonePercentage{};
		int m_RJoystickDeadZonePercentage{};
		int m_LTriggerDeadZonePercentage{};
		int m_RTriggerDeadZonePercentage{};
		/*struct ControllerState
		{
			ControllerButton button;
			KeyState keyState;

			bool operator>(const ControllerState other) const
			{
				if (button != other.button)
					return button > other.button;
				return keyState > other.keyState;
			}

			bool operator<(const ControllerState other) const
			{
				if (button != other.button)
					return button < other.button;
				return keyState < other.keyState;
			}

			bool operator==(const ControllerState& other) const
			{
				return other.button == this->button && other.keyState == this->keyState;
			}

			bool operator!=(const ControllerState& other) const
			{
				return other.button != this->button || other.keyState != this->keyState;
			}
		};

		struct SharedControllerCommand
		{
			std::shared_ptr<Command> pCommand;
			bool active;
		};

		std::map<ControllerState, SharedControllerCommand> m_MapCommands{};*/

		static float m_MaxVibrationValue;
		static float m_MaxJoystickValue;
		static float m_MaxTriggerValue;

		//void HandleInputImpl() const;
	};


	float Controller::ControllerImpl::m_MaxVibrationValue = static_cast<float>(USHRT_MAX);
	float Controller::ControllerImpl::m_MaxJoystickValue = static_cast<float>(SHRT_MAX);
	float Controller::ControllerImpl::m_MaxTriggerValue = static_cast<float>(_UI8_MAX);


	//void Controller::ControllerImpl::HandleInputImpl() const
	//{
	//	for (const auto& [controllerState, pSharedCommand] : m_MapCommands)
	//	{
	//		const auto& [pCommand, active] = pSharedCommand;
	//		switch (controllerState.keyState)
	//		{
	//		case KeyState::DownThisFrame:
	//			if (active && IsDownThisFrameImpl(controllerState.button)) pCommand->Execute();
	//			break;
	//		case KeyState::UpThisFrame:
	//			if (active && IsUpThisFrameImpl(controllerState.button)) pCommand->Execute();
	//			break;
	//		case KeyState::Pressed:
	//			if (active && IsPressedImpl(controllerState.button)) pCommand->Execute();
	//			break;
	//		case KeyState::NotPressed:
	//			if (active && !IsPressedImpl(controllerState.button)) pCommand->Execute();
	//			break;
	//		}
	//	}
	//}
	bool Controller::ControllerImpl::IsAnyButtonPressedImpl()
	{
		XINPUT_STATE previousState = m_CurrentState;
		XINPUT_STATE currentState = XINPUT_STATE{};
		XInputGetState(m_ControllerIndex, &currentState);

		//buttons
		auto buttonChanges = currentState.Gamepad.wButtons ^ previousState.Gamepad.wButtons;

		//Joysticks
        float xL = currentState.Gamepad.sThumbLX / m_MaxJoystickValue;
        float yL = currentState.Gamepad.sThumbLY / m_MaxJoystickValue;

        float xR = currentState.Gamepad.sThumbRX / m_MaxJoystickValue;
        float yR = currentState.Gamepad.sThumbRY / m_MaxJoystickValue;

        if (std::abs(xL) < m_LJoystickDeadZonePercentage / 100.f) xL = 0;
        if (std::abs(yL) < m_LJoystickDeadZonePercentage / 100.f) yL = 0;
        if (std::abs(xR) < m_RJoystickDeadZonePercentage / 100.f) xR = 0;
        if (std::abs(yR) < m_RJoystickDeadZonePercentage / 100.f) yR = 0;

        //Triggers
        float valueL = currentState.Gamepad.bLeftTrigger / m_MaxTriggerValue;
        float valueR = currentState.Gamepad.bRightTrigger / m_MaxTriggerValue;

		if (valueL < m_LTriggerDeadZonePercentage / 100.f) valueL = 0;
		if (valueR < m_RTriggerDeadZonePercentage / 100.f) valueR = 0;

		constexpr float pressTreshhold{ 0.2f };
		return (buttonChanges ||
			xL >= pressTreshhold ||
			yL >= pressTreshhold ||
			xR >= pressTreshhold ||
			yR >= pressTreshhold ||
			valueL >= pressTreshhold ||
			valueR >= pressTreshhold);
	}
	void Controller::ControllerImpl::ProcessInputImpl()
	{
		m_PreviousState = m_CurrentState;
		m_CurrentState = XINPUT_STATE{};
		XInputGetState(m_ControllerIndex, &m_CurrentState);

		auto buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
		m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
		m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
		//HandleInputImpl();
	}

	bool Controller::ControllerImpl::IsDownThisFrameImpl(Button button) const
	{
		return m_ButtonsPressedThisFrame & static_cast<int>(button);
	}
	bool Controller::ControllerImpl::IsUpThisFrameImpl(Button button) const
	{
		return m_ButtonsReleasedThisFrame & static_cast<int>(button);
	}
	bool Controller::ControllerImpl::IsPressedImpl(Button button) const
	{
		return m_CurrentState.Gamepad.wButtons & static_cast<int>(button);
	}

	void Controller::ControllerImpl::SetJoystickDeadzone(bool left, int percentage)
	{
		assert((percentage >= 0 && percentage <= 100) && _T("Percentage value needs to be between 0 and 100."));
		if (left) m_LJoystickDeadZonePercentage = percentage;
		else m_RJoystickDeadZonePercentage = percentage;
	}

	void Controller::ControllerImpl::SetTriggerDeadzone(bool left, int percentage)
	{
		assert((percentage >= 0 && percentage <= 100) && _T("Percentage value needs to be between 0 and 100."));
		if (left) m_LTriggerDeadZonePercentage = percentage;
		else m_RTriggerDeadZonePercentage = percentage;
	}

	//void Controller::ControllerImpl::AddCommandImpl(const std::shared_ptr<Command>& pCommand, ControllerButton button, KeyState keyState)
	//{
	//	ControllerState state{};
	//	state.button = button;
	//	state.keyState = keyState;
	//#ifndef NDEBUG
	//	if (m_MapCommands.contains(state)) std::cout << "Binding to the requested button already exists. Overwriting now.\n";
	//#endif // !NDEBUG
	//	m_MapCommands[state] = SharedControllerCommand{ pCommand, true };
	//}
	//
	//void Controller::ControllerImpl::RemoveCommandImpl(ControllerButton button, KeyState keyState)
	//{
	//	ControllerState state{};
	//	state.button = button;
	//	state.keyState = keyState;
	//	if (m_MapCommands.contains(state)) m_MapCommands.erase(state);
	//}
	//
	//void Controller::ControllerImpl::RemoveAllCommandsImpl()
	//{
	//	m_MapCommands.clear();
	//	VibrateImpl(0);
	//}
	//
	//void Controller::ControllerImpl::DeactivateAllCommandsImpl()
	//{
	//	for (auto& [keyState, pSharedCommand] : m_MapCommands)
	//	{
	//		pSharedCommand.active = false;
	//	}
	//}
	//
	//void Controller::ControllerImpl::ActivateAllCommandsImpl()
	//{
	//	for (auto& [keyState, pSharedCommand] : m_MapCommands)
	//	{
	//		pSharedCommand.active = true;
	//	}
	//}
	//

	void Controller::ControllerImpl::VibrateImpl(int strengthPercentage) const
	{
		assert((strengthPercentage >= 0 && strengthPercentage <= 100) && _T("Percentage value needs to be between 0 and 100."));
		XINPUT_VIBRATION vibration;
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
		float percentage{ strengthPercentage / 100.f };
		vibration.wLeftMotorSpeed = static_cast<WORD>(m_MaxVibrationValue * percentage);
		vibration.wRightMotorSpeed = static_cast<WORD>(m_MaxVibrationValue * percentage);
		XInputSetState(m_ControllerIndex, &vibration);
	}

	Vector2f Controller::ControllerImpl::GetJoystickValueImpl(bool leftJoystick) const
	{
		float x, y;
		if (leftJoystick)
		{
			x = m_CurrentState.Gamepad.sThumbLX / m_MaxJoystickValue;
            y = m_CurrentState.Gamepad.sThumbLY / m_MaxJoystickValue;
			if (std::abs(x) < m_LJoystickDeadZonePercentage / 100.f) x = 0;
			if (std::abs(y) < m_LJoystickDeadZonePercentage / 100.f) y = 0;
		}
		else
		{
			x = m_CurrentState.Gamepad.sThumbRX / m_MaxJoystickValue;
			y = m_CurrentState.Gamepad.sThumbRY / m_MaxJoystickValue;
			if (std::abs(x) < m_RJoystickDeadZonePercentage / 100.f) x = 0;
			if (std::abs(y) < m_RJoystickDeadZonePercentage / 100.f) y = 0;
		}

		return Vector2f{ x, -y };
	}

	float Controller::ControllerImpl::GetTriggerValueImpl(bool leftJoystick) const
	{
		float value;
		if (leftJoystick)
		{
			value = m_CurrentState.Gamepad.bLeftTrigger / m_MaxTriggerValue;
			if (value < m_LTriggerDeadZonePercentage / 100.f) value = 0;
		}
		else
		{
			value = m_CurrentState.Gamepad.bRightTrigger / m_MaxTriggerValue;
			if (value < m_RTriggerDeadZonePercentage / 100.f) value = 0;
		}

		return value;
	}

	int Controller::ControllerImpl::AmountOfConnectedControllersImpl()
	{
		int connectedControllers = 0;
        XINPUT_STATE state{};

        for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            if (const DWORD dwResult = XInputGetState(i, &state); dwResult == ERROR_SUCCESS)
                ++connectedControllers;
		}
		return connectedControllers;
	}

	//Controller
	Controller::Controller(uint8_t controllerIndex) :
		m_pImpl{ new ControllerImpl{controllerIndex} }
	{}

	Controller::~Controller()
	{
		delete m_pImpl;
	}

	bool Controller::IsAnyButtonPressed()
	{
		return m_pImpl->IsAnyButtonPressedImpl();
	}

	void Controller::ProcessControllerInput()
	{
		m_pImpl->ProcessInputImpl();
	}

	bool Controller::IsDownThisFrame(Button button) const
	{
		return m_pImpl->IsDownThisFrameImpl(button);
	}

	bool Controller::IsUpThisFrame(Button button) const
	{
		return m_pImpl->IsUpThisFrameImpl(button);
	}
	bool Controller::IsPressed(Button button) const
	{
		return m_pImpl->IsPressedImpl(button);
	}

	void Controller::SetJoystickDeadzone(bool left, int percentage)
	{
		m_pImpl->SetJoystickDeadzone(left, percentage);
	}
	void Controller::SetTriggerDeadzone(bool left, int percentage)
	{
		m_pImpl->SetTriggerDeadzone(left, percentage);
	}
	//void Controller::AddCommand(const std::shared_ptr<Command>& pCommand, ControllerButton button, KeyState keyState)
	//{
	//	m_pImpl->AddCommandImpl(pCommand, button, keyState);
	//}
	//
	//void Controller::RemoveCommand(ControllerButton button, KeyState keyState)
	//{
	//	m_pImpl->RemoveCommandImpl(button, keyState);
	//}
	//
	//void Controller::RemoveAllCommands()
	//{
	//	m_pImpl->RemoveAllCommandsImpl();
	//}
	//
	//void Controller::DeactivateAllCommands()
	//{
	//	m_pImpl->DeactivateAllCommandsImpl();
	//}
	//
	//void Controller::ActivateAllCommands()
	//{
	//	m_pImpl->ActivateAllCommandsImpl();
	//}

	void Controller::Vibrate(int strengthPercentage) const
	{
		m_pImpl->VibrateImpl(strengthPercentage);
	}

	Vector2f Controller::GetJoystickValue(bool leftJoystick) const
	{
		return m_pImpl->GetJoystickValueImpl(leftJoystick);
	}

	float Controller::GetTriggerValue(bool leftTrigger) const
	{
		return m_pImpl->GetTriggerValueImpl(leftTrigger);
	}

	int Controller::AmountOfConnectedControllers()
	{
		return ControllerImpl::AmountOfConnectedControllersImpl();
	}
}
