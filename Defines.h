#ifndef DEFINES_H
#define DEFINES_H


#include <tchar.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#ifdef _UNICODE								
	#define tstring			std::wstring
	#define tcin			std::wcin
	#define tcout			std::wcout
	#define tstringstream	std::wstringstream
	#define tofstream		std::wofstream
	#define tifstream		std::wifstream
	#define tfstream		std::wfstream
	#define tostream		std::wostream
	#define to_tstring		std::to_wstring
#else
	#define tstring			std::string
	#define tcin			std::cin
	#define tcout			std::cout
	#define tstringstream	std::stringstream
	#define tofstream		std::ofstream
	#define tifstream		std::ifstream
	#define tfstream		std::fstream
	#define tostream		std::ostream
	#define to_tstring		std::to_string
#endif



// Direct2D
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

template<typename Interface>
inline void SafeRelease(
	Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugString("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif



#define ENGINE (Engine::GetSingleton())

#endif // !DEFINES_H