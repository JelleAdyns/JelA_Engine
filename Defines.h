#pragma once
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

#define ENGINE (Engine::GetSingleton())