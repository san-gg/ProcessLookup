#pragma once
#ifndef __ARGUMENTS_
#define __ARGUMENTS_

#include <sstream>

#ifdef _WIN64
#include <Windows.h>
class ArgumentParser {
	DWORD timeGranularity;
	DWORD totalTime;
	DWORD noOfEntries;
	std::stringstream ss;
	
	DWORD strcompare(LPCSTR, LPCSTR);
	DWORD argvalue(LPCSTR, LPCSTR, LPCSTR, LPDWORD);
	DWORD argvalue(LPCSTR, LPCSTR, LPCSTR);
	VOID printHelp();
public:
	ArgumentParser();
	DWORD getTotalTime();
	DWORD getGranularity();
	DWORD parseAg(DWORD, LPSTR*);
	VOID getExecutionParam(LPDWORD, LPDWORD, LPDWORD);
};
#endif

#endif // !__ARGUMENTS_

