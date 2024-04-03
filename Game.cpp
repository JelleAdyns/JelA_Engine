#include "Game.h"

Game::Game()
{

}
Game::~Game()
{
	delete m_Texture;
}
void Game::Initialize()
{
	ENGINE->SetTitle(L"Game");
	ENGINE->SetWindowDimensions(width, height);
	ENGINE->SetBackGroundColor(RGB(0, 0, 0));
	ENGINE->SetFrameRate(120);
}
void Game::Draw() const
{
	ENGINE->SetColor(RGB(255, 255, 255));
	ENGINE->DrawRectangle(0, 0, width - 1, height);
	ENGINE->DrawLine(20, 20, width - 20, height - 20);
	ENGINE->DrawLine(20, 20, width - 20, 20);
	ENGINE->DrawLine(200, 100, 200, height);

	ENGINE->Scale(1.25f,0,0);
	ENGINE->Translate(200,100);
	ENGINE->Rotate(angle, 200,100, false);
	ENGINE->FillRectangle(0, 0, 230, 200);
	ENGINE->FillRoundedRect(250, 0, 230, 200, 80, 80);
	ENGINE->EndTransform();

	ENGINE->FillEllipse(Point2Int{ width / 2, height - 100 }, 50, 60);
	ENGINE->DrawRectangle(width / 2, 0, width, height);
	
	ENGINE->SetColor(RGB(255, 0, 0));
	ENGINE->DrawLine(m_Y, height, m_Y, 0);
	m_Texture->DrawTexture(Point2Int{ width / 2,int(spritePos) }, RectInt{ 24,0,12,27 });
	ENGINE->SetColor(RGB(23, 56, 233));

	ENGINE->Rotate(angle, 0, 20, false);
	ENGINE->DrawString(L"kaas",m_pFont,0,20,400,true);
	ENGINE->EndTransform();
	ENGINE->DrawString(L"kaas",m_pFont2, 400,20,400,true);
}
void Game::Tick(float elapsedSec)
{
	if (GetKeyState(VK_RMENU) & 0x8000)
	{
		OutputDebugString(L"kaas\n");
	}
	m_X += elapsedSec;
	if (m_Y <= 0.f)
	{
		maxVelocity -= 5;
		velocity = maxVelocity;
	}
	velocity += acceleration * elapsedSec;
	m_Y += velocity * elapsedSec;
	if (ENGINE->IsKeyPressed('A')) spritevelocity = height;
	//else spritevelocity = 0;
	spritePos += spritevelocity * elapsedSec;
	spritevelocity = 0;

	angle += 60*elapsedSec;
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
	if (virtualKeycode == 'H') M_pAudio->DecrementVolume();
	if (virtualKeycode == 'J') M_pAudio->IncrementVolume();
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
	
	if (virtualKeycode == 'B') if(!M_pAudio->IsPlaying()) M_pAudio->Play(true);
	if (virtualKeycode == 'Q') if(M_pAudio->IsPlaying()) M_pAudio->SetVolume(20);
	if (virtualKeycode == 'W') if(M_pAudio->IsPlaying()) M_pAudio->SetVolume(80);
	if (virtualKeycode == 'L') M_pAudio2->SetVolume(80);
	if (virtualKeycode == 'V') M_pAudio2->Play(false);
	if (virtualKeycode == 'C')
	{
		//M_pAudio->Pause();
		delete M_pAudio;
		M_pAudio = nullptr;
	}

}
void Game::MouseDown(bool isLeft, int x, int y)
{

}
void Game::MouseUp(bool isLeft, int x, int y)
{
	
}
void Game::MouseMove(int x, int y, int keyDown)
{
	//See this link to check which keys could be represented in the keyDown parameter
	//https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousemove
}
void Game::MouseWheelTurn(int x, int y, int turnDistance, int keyDown)
{
	//See this link to check which keys could be represented in the keyDown parameter and what the turnDistance is for
	//https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel

	OutputDebugString((to_tstring(turnDistance) + L'\n').c_str());
	if (keyDown == MK_LBUTTON)
	{
		OutputDebugString(_T("joew"));
	}
}