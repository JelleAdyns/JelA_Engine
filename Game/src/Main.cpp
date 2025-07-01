#ifdef _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif // _DEBUG

#include "Engine.h"
#include "Game.h"

jela::Engine ENGINE{};

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{  
    bool ok = ENGINE.Init(hInstance, _T("Resources/"), 1200, 720, RGB(0,0,0), _T("Game"));
    int result{};

    if (ok) result = ENGINE.Run(std::make_unique<Game>());
    else { result = -1; }

    ENGINE.Shutdown();

    return result;
}
