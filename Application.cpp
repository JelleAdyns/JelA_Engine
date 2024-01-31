#include "Application.h"
#include "Engine.h"

void Application::Initialize()
{
	ENGINE->SetTitle(L"Hallokes");
	ENGINE->SetWindowDimensions({ 1000, 500 });
}
void Application::Paint()
{
	ENGINE->SetColor(RGB(0, 0, 0));
	ENGINE->SetColor(RGB(255, 255, 255));
	ENGINE->PaintLine(POINT{m_point.x, 234}, POINT{424, m_point.x});
}
void Application::Tick()
{
	m_X += 0.001f;
	m_point.x = cosf(m_X) * 100 + 100;
}