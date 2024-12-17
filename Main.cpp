#ifdef _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif // _DEBUG

#include "Engine.h"
#include "Game.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{  
    int result = ENGINE.Run(_T("../../Resources"), std::make_unique<Game>());
    return result;
}
