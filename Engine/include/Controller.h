
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Command.h"
#include "Structs.h"

namespace jela
{
	class Controller final
	{
	public:

		using ID = uint8_t;

		enum class Button
		{
			DpadUp = 0x0001,
			DpadDown = 0x0002,
			DpadLeft = 0x0004,
			DpadRight = 0x0008,
			Start = 0x0010,
			Back = 0x0020,
			LeftThumb = 0x0040,
			RightThumb = 0x0080,
			LeftShoulder = 0x0100,
			RightShoulder = 0x0200,
			A = 0x1000,
			B = 0x2000,
			X = 0x4000,
			Y = 0x8000
		};
		enum class ButtonState
		{
			UpThisFrame,
			DownThisFrame,
			Pressed,
			NotPressed
		};

		Controller(ID controllerIndex);

		~Controller();

		Controller(const Controller&) = delete;
		Controller& operator= (const Controller&) = delete;
		Controller(Controller&&) noexcept = default;
		Controller& operator= (Controller&&) noexcept = default;

		bool IsAnyButtonPressed() const;
		void ProcessControllerInput();
		bool IsDownThisFrame(Button button) const;
		bool IsUpThisFrame(Button button)  const;
		bool IsPressed(Button button)  const;

		void SetJoystickDeadzone(bool left, uint8_t percentage);
		void SetTriggerDeadzone(bool left, uint8_t percentage);
		void AddCommand(const std::shared_ptr<Command>& pCommand, Button button, ButtonState buttonState);
		void RemoveCommand(Button button, ButtonState buttonState);
		void RemoveAllCommands();

		void DeactivateAllCommands();
		void ActivateAllCommands();

		void Vibrate(uint8_t strengthPercentage) const;
		Vector2f GetJoystickValue(bool leftJoystick) const;
		float GetTriggerValue(bool leftTrigger) const;
		static int AmountOfConnectedControllers();

	private:

		class ControllerImpl;
		ControllerImpl* m_pImpl;
	};

}
#endif // !CONTROLLER_H


