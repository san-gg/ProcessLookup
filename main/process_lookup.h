#pragma once
#ifndef __PROCESS_LOOKUP_
#define __PROCESS_LOOKUP_

#ifdef _WIN64

#include "process_map.h"
#include "main_win_lookup.h"
#include <set>
#include <vector>
#include <map>
class WMIProcessList {
	IWbemLocator* pLoc;
	IWbemServices* pSvc;
	IEnumWbemClassObject* pEnumerator;
	DWORD isServerConnected;
	DWORD wmiListFlag;

	DWORD prosQueryLookupAll();
public:
	WMIProcessList();
	//WMIProcessList(WMIProcessList& other) = delete;
	//VOID operator=(const WMIProcessList&) = delete;
	~WMIProcessList();

	INT wmiListInit(WMIListPECKU&);
	INT wmiListNext(WMIListPECKU&);
	DWORD getQueryDataAll(ProcessMap*);
	ULONGLONG cvtBSTR_ulong(LPWSTR);
	HRESULT init();
};

class ListResultWMI {
private:
	struct Result {
		DWORD pid;
		DWORD cpuUsage;
		bool operator<(const Result&) const;
	};
	std::map<DWORD, std::vector<DWORD>> topResults2;
	//std::unordered_map<DWORD, MinProcessProp> curlist;
	ProcessMap mainlist;
	WMIProcessList wmiList;

	DWORD topEntries;
	DWORD maxEntries; 
	DWORD totalProcess;
	DWORD updateId;
	ULONGLONG sysKernelTime, sysUserTime;
	ULONGLONG c_sysKernelTime, c_sysUserTime;
	
	VOID forceAddResult(DWORD, DWORD);
	//update kernel time
	DWORD setTimes(PULONGLONG, PULONGLONG);
	VOID updateTotalProcess();
public:
	ListResultWMI();
	~ListResultWMI();

	VOID updateResult2(DWORD, DWORD);

	//init
	DWORD init();

	// check in topPID
	DWORD update_pick();

	//no of entries
	VOID setEntries(DWORD);

	// Log Result
	//VOID logit();
	VOID printResult();
};
#endif // _WIN64

#endif // !__PROCESS_LOOKUP_
