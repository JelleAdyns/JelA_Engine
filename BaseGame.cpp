#include "Engine.h"
#include "BaseGame.h"

void BaseGame::Initialize()
{
    ENGINE.SetTitle(_T("Game"));
    ENGINE.SetColor(RGB(255, 255, 255));
    ENGINE.SetWindowDimensions(1200, 720); 
    ENGINE.SetBackGroundColor(RGB(0, 0, 0));
    ENGINE.SetFrameRate(60);
}
