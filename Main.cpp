#ifndef NDEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif // !NDEBUG

#include "Engine.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    // Use HeapSetInformation to specify that the process should terminate if the heap manager detects an error in any heap used by the process.
    // The return value is ignored, because we want to continue running in the unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
    int result = 0;
    if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)) && SUCCEEDED(MFStartup(MF_VERSION)))
    {
        ENGINE.SetInstance(HINST_THISCOMPONENT);
        ENGINE.SetResourcePath(_T("../../Resources/"));
        result = ENGINE.Run();
        CoUninitialize();
    }
    return result;
}
