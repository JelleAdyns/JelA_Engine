#include "Controller.h"
#include <unordered_map>

#include <XInput.h>

namespace jela
{
	//Implementation
	class Controller::ControllerImpl
	{
	public:
		explicit ControllerImpl(uint8_t controllerIndex) :
			m_ControllerIndex{ controllerIndex }
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


		bool IsAnyButtonPressedImpl() const;
		void ProcessInputImpl();
		bool IsDownThisFrameImpl(Button button) const;
		bool IsUpThisFrameImpl(Button button)  const;
		bool IsPressedImpl(Button button)  const;

		void SetJoystickDeadzone(bool left, uint8_t percentage);
		void SetTriggerDeadzone(bool left, uint8_t percentage);

		void AddCommandImpl(const std::shared_ptr<Command>& pCommand, Button button, ButtonState buttonState);
		void RemoveCommandImpl(Button button, ButtonState buttonState);
		void RemoveAllCommandsImpl();

		void DeactivateAllCommandsImpl();
		void ActivateAllCommandsImpl();

		void VibrateImpl(uint8_t strengthPercentage) const;
		Vector2f GetJoystickValueImpl(bool leftJoystick) const;
		float GetTriggerValueImpl(bool leftJoystick) const;

		static int AmountOfConnectedControllersImpl();

	private:


		struct ControllerButtonState
		{
			Button button;
			ButtonState buttonState;

			bool operator==(const ControllerButtonState& other) const
			{
				return button == other.button && buttonState == other.buttonState;
			}
		};

		struct StateHasher
		{
			std::size_t operator()(const ControllerButtonState& controllerState) const
			{
				auto result = static_cast<std::size_t>(controllerState.buttonState) << 8;
				result |= static_cast<std::size_t>(controllerState.button);

				return result;
			}
		};

		using SharedControllerCommand = std::shared_ptr<Command>;

		std::unordered_map<ControllerButtonState, SharedControllerCommand, StateHasher> m_MapCommands{};
		XINPUT_STATE m_PreviousState{};
		XINPUT_STATE m_CurrentState{};
		uint32_t m_ButtonsPressedThisFrame{};
		uint32_t m_ButtonsReleasedThisFrame{};
		uint8_t m_LJoystickDeadZonePercentage{};
		uint8_t m_RJoystickDeadZonePercentage{};
		uint8_t m_LTriggerDeadZonePercentage{};
		uint8_t m_RTriggerDeadZonePercentage{};
		uint8_t m_ControllerIndex{};
		bool m_AreCommandsActive{true};

		static constexpr float m_MaxVibrationValue{static_cast<float>(USHRT_MAX)};
		static constexpr float m_MaxJoystickValue{static_cast<float>(SHRT_MAX)};
		static constexpr float m_MaxTriggerValue{static_cast<float>(_UI8_MAX)};

		void HandleInputImpl() const;
	};


	void Controller::ControllerImpl::HandleInputImpl() const
	{
		if (!m_AreCommandsActive) return;

		for (const auto& [controllerState, pSharedCommand] : m_MapCommands)
		{
			switch (controllerState.buttonState)
			{
			case ButtonState::DownThisFrame:
				if (IsDownThisFrameImpl(controllerState.button)) pSharedCommand->Execute();
				break;
			case ButtonState::UpThisFrame:
				if (IsUpThisFrameImpl(controllerState.button)) pSharedCommand->Execute();
				break;
			case ButtonState::Pressed:
				if (IsPressedImpl(controllerState.button)) pSharedCommand->Execute();
				break;
			case ButtonState::NotPressed:
				if (!IsPressedImpl(controllerState.button)) pSharedCommand->Execute();
				break;
			}
		}
	}
	bool Controller::ControllerImpl::IsAnyButtonPressedImpl() const
	{
		const XINPUT_STATE previousState = m_CurrentState;
		XINPUT_STATE currentState = XINPUT_STATE{};
		XInputGetState(m_ControllerIndex, &currentState);

		//buttons
		const auto buttonChanges = currentState.Gamepad.wButtons ^ previousState.Gamepad.wButtons;

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
		HandleInputImpl();
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

	void Controller::ControllerImpl::SetJoystickDeadzone(bool left, uint8_t percentage)
	{
		assert((percentage <= 100) && _T("Percentage value needs to be between 0 and 100."));
		if (left) m_LJoystickDeadZonePercentage = percentage;
		else m_RJoystickDeadZonePercentage = percentage;
	}

	void Controller::ControllerImpl::SetTriggerDeadzone(bool left, uint8_t percentage)
	{
		assert((percentage <= 100) && _T("Percentage value needs to be between 0 and 100."));
		if (left) m_LTriggerDeadZonePercentage = percentage;
		else m_RTriggerDeadZonePercentage = percentage;
	}

	void Controller::ControllerImpl::AddCommandImpl(const std::shared_ptr<Command>& pCommand, Button button, ButtonState buttonState)
	{
		ControllerButtonState state{};
		state.button = button;
		state.buttonState = buttonState;
	#ifndef NDEBUG
		if (m_MapCommands.contains(state)) std::cout << "Binding to the requested button already exists. Overwriting now.\n";
	#endif // !NDEBUG
		m_MapCommands[state] = pCommand;
	}

	void Controller::ControllerImpl::RemoveCommandImpl(Button button, ButtonState buttonState)
	{
		ControllerButtonState state{};
		state.button = button;
		state.buttonState = buttonState;
		if (m_MapCommands.contains(state)) m_MapCommands.erase(state);
	}

	void Controller::ControllerImpl::RemoveAllCommandsImpl()
	{
		m_MapCommands.clear();
		VibrateImpl(0);
	}

	void Controller::ControllerImpl::DeactivateAllCommandsImpl()
	{
		m_AreCommandsActive = false;
	}

	void Controller::ControllerImpl::ActivateAllCommandsImpl()
	{
		m_AreCommandsActive = true;
	}


	void Controller::ControllerImpl::VibrateImpl(uint8_t strengthPercentage) const
	{
		assert((strengthPercentage <= 100) && _T("Percentage value needs to be between 0 and 100."));
		XINPUT_VIBRATION vibration;
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
		const float percentage{ strengthPercentage / 100.f };
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
		float value{};
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
	Controller::Controller(ID controllerIndex) :
		m_pImpl{ new ControllerImpl{controllerIndex} }
	{}

	Controller::~Controller()
	{
		delete m_pImpl;
	}

	bool Controller::IsAnyButtonPressed() const
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

	void Controller::SetJoystickDeadzone(bool left, uint8_t percentage)
	{
		m_pImpl->SetJoystickDeadzone(left, percentage);
	}
	void Controller::SetTriggerDeadzone(bool left, uint8_t percentage)
	{
		m_pImpl->SetTriggerDeadzone(left, percentage);
	}
	void Controller::AddCommand(const std::shared_ptr<Command>& pCommand, Button button, ButtonState buttonState)
	{
		m_pImpl->AddCommandImpl(pCommand, button, buttonState);
	}

	void Controller::RemoveCommand(Button button, ButtonState buttonState)
	{
		m_pImpl->RemoveCommandImpl(button, buttonState);
	}

	void Controller::RemoveAllCommands()
	{
		m_pImpl->RemoveAllCommandsImpl();
	}

	void Controller::DeactivateAllCommands()
	{
		m_pImpl->DeactivateAllCommandsImpl();
	}

	void Controller::ActivateAllCommands()
	{
		m_pImpl->ActivateAllCommandsImpl();
	}

	void Controller::Vibrate(uint8_t strengthPercentage) const
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
