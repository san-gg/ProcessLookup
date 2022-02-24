#pragma once
#ifndef __PROCESS_BLOCK_
#define __PROCESS_BLOCK_

#ifdef _WIN64
#include "main_win_lookup.h"
struct ProcessProp {
	ULONGLONG procKernel;
	ULONGLONG procUser;
	LPWSTR commandLine;
	LPWSTR exePath;
	DWORD pid;
	//DWORD cpuUsage;
	DWORD metaData;
	ProcessProp() : procKernel(0), procUser(0), pid(0), exePath(0), commandLine(0), metaData(0) { }
};
struct MinProcessProp {
	ULONGLONG procKernel;
	ULONGLONG procUser;
	DWORD pid;
	DWORD uid;
	MinProcessProp() : procKernel(0), procUser(0), pid(0), uid(0) { }
};
struct WMIListPECKU {
	VARIANT ProcessId, ExecutablePath, CommandLine, KernelModeTime, UserModeTime;
	DWORD ret;
};
#endif

#endif // !__PROCESS_BLOCK_
