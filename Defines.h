#ifndef DEFINES_H
#define DEFINES_H


#include <tchar.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

#ifdef _UNICODE								
	#define tchar			wchar_t
	#define tstring			std::wstring
	#define tcin			std::wcin
	#define tcout			std::wcout
	#define tstringstream	std::wstringstream
	#define tofstream		std::wofstream
	#define tifstream		std::wifstream
	#define tfstream		std::wfstream
	#define tostream		std::wostream
	#define to_tstring		std::to_wstring

	#define tregex			std::wregex
	#define tcmatch			std::wcmatch
	#define tsmatch			std::wsmatch
	#define tcsub_match		std::wcsub_match
	#define tssub_match		std::wssub_match
#else
	#define tchar			char
	#define tstring			std::string
	#define tcin			std::cin
	#define tcout			std::cout
	#define tstringstream	std::stringstream
	#define tofstream		std::ofstream
	#define tifstream		std::ifstream
	#define tfstream		std::fstream
	#define tostream		std::ostream
	#define to_tstring		std::to_string

	#define tregex			std::regex
	#define tcmatch			std::cmatch
	#define tsmatch			std::smatch
	#define tcsub_match		std::csub_match
	#define tssub_match		std::ssub_match
#endif

//next ifdef is code from Kevin Hoefman, teacher at Howest, DAE in Kortrijk
//64 bit defines
#ifdef _WIN64
#define GWLA_WNDPROC	GWLP_WNDPROC
#define GWLA_HINSTANCE	GWLP_HINSTANCE
#define GWLA_HWNDPARENT GWLP_HWNDPARENT
#define GWLA_USERDATA	GWLP_USERDATA
#else if
#define GWLA_WNDPROC	GWL_WNDPROC
#define GWLA_HINSTANCE	GWL_HINSTANCE
#define GWLA_HWNDPARENT GWL_HWNDPARENT
#define GWLA_USERDATA	GWL_USERDATA
#endif


// DirectX
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <dwrite_3.h>
#include <mfmediaengine.h>
#include <Audioclient.h>
#include <mfapi.h>
#include <Mfidl.h>
#include <mferror.h>
#include <MMSystem.h>
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

#define MATHEMATICAL_COORDINATESYSTEM


//#include "Engine.h"

#define ENGINE (Engine::GetSingleton())

#endif // !DEFINES_H