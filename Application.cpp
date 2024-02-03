#include "Application.h"
#include "Engine.h"

void Application::Initialize()
{
	ENGINE->SetTitle(L"Hallokes");
	ENGINE->SetWindowDimensions( width, height);
	ENGINE->SetFrameRate(60);
}
void Application::Draw()
{
	ENGINE->SetColor(RGB(255, 255, 255));
	ENGINE->DrawRectangle(0, 0, width-1, height);
	//ENGINE->DrawRectangle(width/2.f, 0, width, height);
}
void Application::Tick()
{
	m_X += 1.f;
	//m_point.x = LONG(cos(m_X) * 100 + 100);
	//m_point.x = m_X;
}