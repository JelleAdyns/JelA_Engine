#ifdef _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif // _DEBUG

#include "Engine.h"

jela::Engine ENGINE{};

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE /*hPrevInstance*/,
    _In_ LPTSTR    /*lpCmdLine*/,
    _In_ int       /*nCmdShow*/)
{  
    int result{-1};

    if (ENGINE.Init(hInstance, _T("Resources/"), 1920, 1080))
    {
        result = ENGINE.Run();
    }

    ENGINE.Shutdown();

    return result;
}
