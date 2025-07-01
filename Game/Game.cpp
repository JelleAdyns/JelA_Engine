#include "Game.h"
#include "Audio.h"

void Game::Initialize()
{
	jela::AudioLocator::RegisterAudioService(std::make_unique<jela::LogAudio>(std::make_unique<jela::Audio>()));
	jela::AudioLocator::GetAudioService().AddSound(_T("SpaceshipOLD.wav"),0);
	jela::AudioLocator::GetAudioService().PlaySoundClip(0, false);

	ENGINE.ResourceMngr()->GetTexture(_T("NES - Bubble Bobble - Level Tiles.png"), m_Texture);
	ENGINE.ResourceMngr()->GetFont(_T("castlevania-3.ttf"), m_Font, true);
	ENGINE.ResourceMngr()->GetFont(_T("Blackadder ITC"), m_Font2);
	m_testTexture = new TestTexture{};
	ENGINE.SetBackGroundColor(RGB(0, 0, 255));
	//ENGINE.SetWindowScale(1.5f);
}
void Game::Cleanup()
{
	if (m_testTexture) delete m_testTexture;
}
void Game::Draw() const
{
	static float pos = 0;
	pos += 20 * ENGINE.GetDeltaTime();
	if (pos > ENGINE.GetWindowRect().width) pos =0;
	//jela::Texture* refPointer = m_RefPointer;
	std::vector<jela::Point2Int> kaas{
	jela::Point2Int{200,200},
	jela::Point2Int{250,300},
	jela::Point2Int{450,250},
	jela::Point2Int{400,200},
	jela::Point2Int{300,150},
	};

	ENGINE.SetColor(RGB(255, 255, 255));
	ENGINE.FillPolygon(kaas);

	ENGINE.DrawArc(600, 500, 300, 150, 20, 190, 1.f, true);
	ENGINE.FillArc(600, 200, 300, 150, 20, 190);
	ENGINE.SetFont(m_Font.pObject);
	ENGINE.SetTextFormat(pTextFormat.get());

	ENGINE.PushTransform();
	ENGINE.Scale(2);

	ENGINE.DrawString(mouseText, 0,0,500,true);
	ENGINE.SetFont(m_Font2.pObject);
	ENGINE.DrawString(mouseText, 500,0,500,true);
	
	ENGINE.PopTransform();

	ENGINE.SetColor(RGB(255, 0, 255));
	ENGINE.DrawTexture(m_Texture.pObject, 100,0);
	ENGINE.FillRectangle(static_cast<int>(pos), 200,20,20);
	
	if(m_testTexture) m_testTexture->Draw();
}
void Game::Tick()
{
	
}
void Game::KeyDown(int virtualKeycode)
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
void Game::KeyDownThisFrame(int virtualKeycode)
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
void Game::KeyUp(int virtualKeycode)
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

	if (virtualKeycode == VK_DOWN)
	{
		ENGINE.ResourceMngr()->RemoveTexture(_T("NES - Bubble Bobble - Level Tiles.png"));
	}
	if (virtualKeycode == VK_SPACE)
	{
		ENGINE.ResourceMngr()->RemoveTexture(_T("Wesley.png"));
	}
	if (virtualKeycode == VK_SPACE)
	{
		ENGINE.ResourceMngr()->RemoveFont(_T("castlevania-3.ttf"));
	}
	if (virtualKeycode == 'H')
	{
		if (m_testTexture)
		{
			delete m_testTexture;
			m_testTexture = nullptr;
		}
	}
}
void Game::HandleControllerInput()
{
}
void Game::MouseDown(bool isLeft, int x, int y)
{

}
void Game::MouseUp(bool isLeft, int x, int y)
{
	
}
void Game::MouseMove(int x, int y, int keyDown)
{
	mouseText = (to_tstring(x) + _T(',') + to_tstring(y));
	//See this link to check which keys could be represented in the keyDown parameter
	//https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousemove
}
void Game::MouseWheelTurn(int x, int y, int turnDistance, int keyDown)
{
	//See this link to check which keys could be represented in the keyDown parameter and what the turnDistance is for
	//https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel
}