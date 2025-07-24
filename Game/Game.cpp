#include "Game.h"
#include "Audio.h"

void Game::Initialize()
{
	
}
void Game::Cleanup()
{
	
}
void Game::Tick()
{
	
}
void Game::Draw() const
{
	
}
void Game::KeyDown(int /*virtualKeycode*/)
{

	// Numbers and letters from '0' to '9' and 'A' to 'Z' are represented by their ASCII values
	// For example: if(virtualKeycode == 'B')
	// BE CAREFULL! Don't use lower caps, because those have different ASCII values
	//
	// Other keys are checked with their virtual Keycode defines
	// For example: if(virtualKeycode == VK_MENU)
	// VK_MENU represents the 'Alt' key
	//
	// Click here for more information: https://learn.microsoft.com/en-us/windows/win32/learnwin32/keyboard-input

}
void Game::KeyDownThisFrame(int /*virtualKeycode*/)
{
	// Numbers and letters from '0' to '9' and 'A' to 'Z' are represented by their ASCII values
	// For example: if(virtualKeycode == 'B')
	// BE CAREFULL! Don't use lower caps, because those have different ASCII values
	//
	// Other keys are checked with their virtual Keycode defines
	// For example: if(virtualKeycode == VK_MENU)
	// VK_MENU represents the 'Alt' key
	//
	// Click here for more information: https://learn.microsoft.com/en-us/windows/win32/learnwin32/keyboard-input

}
void Game::KeyUp(int /*virtualKeycode*/)
{
	// Numbers and letters from '0' to '9' and 'A' to 'Z' are represented by their ASCII values
	// For example: if(virtualKeycode == 'B')
	// BE CAREFULL! Don't use lower caps, because those have different ASCII values
	//
	// Other keys are checked with their virtual Keycode defines
	// For example: if(virtualKeycode == VK_MENU)
	// VK_MENU represents the 'Alt' key
	//
	// Click here for more information: https://learn.microsoft.com/en-us/windows/win32/learnwin32/keyboard-input
	
}
void Game::HandleControllerInput()
{
}
void Game::MouseDown(jela::MouseButtons /*buttonDownThisFrame*/, float /*x*/, float /*y*/, jela::MouseButtons /*allDownButtons*/)
{
}
void Game::MouseUp(jela::MouseButtons /*buttonDownThisFrame*/, float /*x*/, float /*y*/, jela::MouseButtons /*allDownButtons*/)
{
}
void Game::MouseDoubleClick(jela::MouseButtons /*buttonDownThisFrame*/, float /*x*/, float /*y*/, jela::MouseButtons /*allDownButtons*/)
{
}
void Game::MouseMove(float /*x*/, float /*y*/, jela::MouseButtons /*allDownButtons*/)
{
}
void Game::MouseWheelTurn(float /*x*/, float /*y*/, int /*turnDistance*/, jela::MouseButtons /*allDownButtons*/)
{
	//See this link for more information:https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel
}