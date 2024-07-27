#include "Game.h"

Game::Game()
{

}
Game::~Game()
{
	delete m_Texture;
	delete m_pFont;
	delete m_pFont2;
	//delete M_pAudio;
	//delete M_pAudio2;
	//delete M_pAudio3;
}
void Game::Initialize()
{
#ifndef NDEBUG
	AudioLocator::RegisterAudioService(std::make_unique<LogAudio>(std::make_unique<Audio>()));
#else
	AudioLocator::RegisterAudioService(std::make_unique<Audio>());
#endif // !NDEBUG


	ENGINE.SetTitle(_T("Game"));
	ENGINE.SetWindowDimensions(width, height);
	ENGINE.SetBackGroundColor(RGB(0, 0, 0));
	ENGINE.SetFrameRate(120);

	AudioLocator::GetAudioService().AddSound(L"Spaceship.mp3", static_cast<SoundID>(Clips::Spaceship));
	AudioLocator::GetAudioService().AddSound(L"saw_sfx1.wav", static_cast<SoundID>(Clips::Saw));
}
void Game::Draw() const
{
	ENGINE.SetColor(RGB(255, 255, 255));
	ENGINE.DrawRectangle(0, 0, width - 1, height);
	ENGINE.DrawLine(20, 20, width - 20, height - 20);
	ENGINE.DrawLine(20, 20, width - 20, 20);
	ENGINE.DrawLine(200, 100, 200, height);

	ENGINE.Scale(1.25f,0,0);
	ENGINE.Translate(200,100);
	ENGINE.Rotate(angle, 200,100, false);
	ENGINE.FillRectangle(0, 0, 230, 200);
	ENGINE.FillRoundedRect(250, 0, 230, 200, 80, 80);
	ENGINE.EndTransform();

	ENGINE.FillEllipse(Point2Int{ width / 2, height - 100 }, 50, 60);
	ENGINE.DrawRectangle(width / 2, 0, width, height);
	
	ENGINE.SetColor(RGB(255, 0, 0));
	ENGINE.DrawLine(m_Y, height, m_Y, 0);

	ENGINE.Scale(4.f, Point2Int{ width / 2,int(spritePos) });
	ENGINE.Translate(200, 100);
	ENGINE.Rotate(angle, Point2Int{ width / 2,int(spritePos) }, false);
	ENGINE.DrawTexture(*m_Texture, Point2Int{ width / 2,int(spritePos) }, RectInt{ 24,0,12,27 });
	ENGINE.EndTransform();

	ENGINE.SetColor(RGB(23, 56, 233));

	ENGINE.Rotate(angle, 0, 20, false);
	ENGINE.DrawString(L"kaas",m_pFont,0,20,400,true);
	ENGINE.EndTransform();
	ENGINE.DrawString(L"kaas",m_pFont2, 400,20,400,true);


	ENGINE.DrawRectangle(RectInt{ 20,30, 100, 150 });
	ENGINE.DrawLine(Point2Int{ 10,40 }, Point2Int{ 200,179 });
	ENGINE.DrawLine(Point2Int{ 0,0 }, Point2Int{ 20,20 });
	ENGINE.DrawLine(Point2Int{ 10,10 }, Point2Int{ 40,40 });
	ENGINE.SetColor(RGB(0, 255, 0));


	ENGINE.DrawVector(Point2Int{ 100,400 }, Vector2f{ 100,-20 });
	ENGINE.DrawVector(Point2Int{ 100,400 } + Vector2f{ 100,-20 }, Vector2f{ 100,-120 });
	ENGINE.DrawVector(Point2Int{ 100,400 }, Vector2f{ 100,-20 } + Vector2f{ 100,-120 });
	
	ENGINE.DrawVector(origin, dot);
	ENGINE.DrawVector(origin, toDot);

	ENGINE.SetColor(RGB(255, 56, 233));
	ENGINE.DrawVector(origin, first);



}
void Game::Tick()
{
	if (GetKeyState(VK_RMENU) & 0x8000)
	{
		bool testbool = utils::IsPointInRect(Point2Int{ 15,15 }, RectInt{ 0,10,20,20 });
		OutputDebugString(to_tstring(testbool).c_str());
	}
	m_X += ENGINE.GetDeltaTime();
	if (m_Y <= 0.f)
	{
		maxVelocity -= 5;
		velocity = maxVelocity;
	}
	velocity += acceleration * ENGINE.GetDeltaTime();
	m_Y += velocity * ENGINE.GetDeltaTime();
	if (ENGINE.IsKeyPressed('A')) spritevelocity = height;
	//else spritevelocity = 0;
	spritePos += spritevelocity * ENGINE.GetDeltaTime();
	spritevelocity = 0;

	angle += 60 * ENGINE.GetDeltaTime();

	std::pair<Point2Int, Point2Int> p{};
	bool testbool = utils::IntersectRectLine(RectInt{ 20,30, 100, 150 }, Point2Int{ 10,40 }, Point2Int{ 200,179 }, p);

	float la{};
	float la2{};
	bool crosstest = utils::IntersectLineSegments(Point2Int{ 0,0 }, Point2Int{ 20,20 }, Point2Int{ 10,10 }, Point2Int{ 40,40 }, la, la2);

	Point2Int kruispunt = Point2Int{ 0,0 } + Vector2f{ Point2Int{ 0,0 } , Point2Int{ 20,20 } } * la;
	Point2Int kruispunt2 = Point2Int{ 10,10 } + Vector2f{ Point2Int{ 10,10 } , Point2Int{ 40,40 } } * la2;


	toDot.x = cos(angle * float(std::numbers::pi) / 180) * 100;
	toDot.y = sin(angle * float(std::numbers::pi) / 180) * 100;

	Vector2f norm = toDot.Normalized();
	float dotPr = Vector2f::Dot(toDot, first);
	dot = norm * dotPr;

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

	
	if (virtualKeycode == 'H') AudioLocator::GetAudioService().DecrementMasterVolume();//M_pAudio->DecrementVolume();
	if (virtualKeycode == 'J')  AudioLocator::GetAudioService().IncrementMasterVolume(); //M_pAudio->IncrementVolume();
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
	auto& locator = AudioLocator::GetAudioService();
	if (virtualKeycode == 'B') locator.PlaySoundClip(static_cast<SoundID>(Clips::Spaceship), true);
	if (virtualKeycode == 'N') locator.PauseSound(static_cast<SoundID>(Clips::Spaceship));
	if (virtualKeycode == 'M') locator.ResumeSound(static_cast<SoundID>(Clips::Spaceship));
	if (virtualKeycode == 'Z') locator.PlaySoundClip(static_cast<SoundID>(Clips::Saw), false);
	if (virtualKeycode == 'X') locator.PauseSound(static_cast<SoundID>(Clips::Saw));
	if (virtualKeycode == 'C') locator.ResumeSound(static_cast<SoundID>(Clips::Saw));
	if (virtualKeycode == 'R') locator.RemoveSound(static_cast<SoundID>(Clips::Saw));
	//if (virtualKeycode == 'M') if(!M_pAudio3->IsPlaying()) M_pAudio3->Play(false);
	//if (virtualKeycode == 'Q') if(M_pAudio->IsPlaying()) M_pAudio->SetVolume(20);
	//if (virtualKeycode == 'W') if(M_pAudio->IsPlaying()) M_pAudio->SetVolume(80);
	//if (virtualKeycode == 'L') M_pAudio2->SetVolume(80);
	//if (virtualKeycode == 'V') M_pAudio2->Play(false);
	//if (virtualKeycode == 'C')
	//{
	//	M_pAudio->Pause();
	//	//delete M_pAudio;
	//	//M_pAudio = nullptr;
	//}

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