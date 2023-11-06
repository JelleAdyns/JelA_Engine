#include "Application.h"
#include "Engine.h"

void Application::Initialize()
{
	ENGINE->SetTitle(L"Hallokes");
	ENGINE->SetWindowDimensions({ 1000, 500 });
}
void Application::Paint()
{

	ENGINE->PaintLine(POINT{12, 234}, POINT{424, 232});
}
void Application::Tick()
{

}