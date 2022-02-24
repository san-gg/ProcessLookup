#pragma once
#ifndef __PROCESS_MAP_
#define __PROCESS_MAP_

#include <unordered_map>
#include <vector>
#include "process_block.h"

#ifdef _WIN64
#include "main_win_lookup.h"
class ProcessMap {
	ProcessProp* pProp;
	std::unordered_map<DWORD, ProcessProp*> procMap;
	std::vector<LPVOID> extraMemory;
	std::unordered_map<DWORD, ProcessProp*>::iterator itr;
public:
	ProcessMap();
	~ProcessMap();

	DWORD add(ProcessProp*);
	DWORD contains(DWORD);
	DWORD updateProp(ProcessProp*);
	//LPWSTR getMemoryWchar(DWORD);
	VOID addMemoryWChar(LPWSTR);
	VOID begin();
	DWORD next();
	VOID getItrData(ProcessProp*);
	VOID getProp(ProcessProp*);
	VOID getAllProp(ProcessProp*);
	//VOID itrRemove();
	ULONG64 size();
};
#endif // _WIN64

#endif // !__PROCESS_MAP_
