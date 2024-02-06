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
void Game::Draw()
{
	ENGINE->SetColor(RGB(255, 255, 255));
	ENGINE->DrawRectangle(0, 0, width - 1, height);
	ENGINE->DrawLine(20, 20, width - 20, height - 20);
	ENGINE->DrawLine(20, 20, width - 20, 20);
	ENGINE->FillRectangle(30, 0, 230, 200);
	//ENGINE->FillRoundedRect(250, 0, 230, 200, 80, 80);
	ENGINE->FillEllipse(Point2Int{ width / 2, height - 100 }, 50, 60);
	ENGINE->DrawRectangle(width / 2, 0, width, height);
	
	ENGINE->SetColor(RGB(255, 0, 0));
	ENGINE->DrawLine(m_Y, height, m_Y, 0);
	ENGINE->DrawTexture(*m_Texture, Point2Int{ width/2,20 }, RectInt{ 24,0,12,27 });
}
void Game::Tick(float elapsedSec)
{
	m_X += elapsedSec;
	if (m_Y <= 0.f)
	{
		maxVelocity -= 5;
		velocity = maxVelocity;
	}
	velocity += acceleration * elapsedSec;
	m_Y += velocity * elapsedSec;

}