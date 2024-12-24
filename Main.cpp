#ifdef _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif // _DEBUG

#include "Engine.h"
#include "Game.h"

int APIENTRY WINAPI _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{  
    bool ok = jela::Engine::GetInstance().Init(hInstance, _T("Resources/"), 1200, 720, RGB(0,0,0), _T("Game"));
    if (ok)
    {
        int result = jela::Engine::GetInstance().Run(std::make_unique<Game>());
        jela::Engine::Shutdown();

        return result;
    }
    return -1;
}
