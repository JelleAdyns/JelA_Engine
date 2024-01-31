#include "Application.h"
#include "Engine.h"

void Application::Initialize()
{
	ENGINE->SetTitle(L"Hallokes");
	ENGINE->SetWindowDimensions({ 1200, 640});
}
void Application::Paint()
{
	//ENGINE->SetColor(RGB(0, 0, 0));
	ENGINE->SetColor(RGB(255, 255, 255),0.5F);
	ENGINE->PaintLine(m_point.x, 234, 424, m_point.x);
}
void Application::Tick()
{
	m_X += 0.1f;
	m_point.x = LONG(cosf(m_X) * 100 + 100);
}