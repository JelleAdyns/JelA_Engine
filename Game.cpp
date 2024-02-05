#include "Game.h"

Game::Game()
{

}
Game::~Game()
{

}
void Game::Initialize()
{
	ENGINE->SetTitle(L"Hallokes");
	ENGINE->SetWindowDimensions(width, height);
	ENGINE->SetBackGroundColor(RGB(0, 155, 0));
	ENGINE->SetFrameRate(120);
}
void Game::Draw()
{
	ENGINE->SetColor(RGB(255, 255, 255));
	ENGINE->DrawRectangle(0, 0, width - 1, height);
	ENGINE->DrawLine(20, 20, width - 20, height - 20);
	ENGINE->FillRectangle(30, 0, 230, 200);
	ENGINE->FillRoundedRect(250, 0, 230, 200, 80, 80);
	ENGINE->FillEllipse(Point2Int{ width / 2, height - 100 }, 50, 60);
	ENGINE->DrawRectangle(width / 2, 0, width, height);

	ENGINE->SetColor(RGB(255, 0, 0));
	ENGINE->DrawLine(m_X, height, m_Y, 0);
	m_Texture.Draw();
}
void Game::Tick(float elapsedSec)
{
	m_X += elapsedSec;
	if (m_Y <= 0.f)
	{
		maxVelocity -= 1;
		velocity = maxVelocity;
	}
	velocity += acceleration * elapsedSec;
	m_Y += velocity * elapsedSec;

}