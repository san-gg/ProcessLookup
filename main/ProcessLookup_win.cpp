#include <iostream>
#include "process_map.h"
#include "process_lookup.h"
//#define __DEBUG_

VOID copyStringBSTR(LPWSTR* des, BSTR source) {
	ULONG len = 0;
	LPWSTR wChar = 0;
	*des = 0;
	if (source != 0) {
		len = SysStringLen(source) + 1;
		wChar = new (std::nothrow) WCHAR[len];
		if (wChar != 0) {
			for (int i = 0; i < len; i++) {
				wChar[i] = source[i];
			}
			*des = wChar;
		}
	}
}

WMIProcessList::WMIProcessList() : pLoc(0), pSvc(0), pEnumerator(0), isServerConnected(0), wmiListFlag(0) { }

WMIProcessList::~WMIProcessList() {
	if (this->isServerConnected == 1) {
		this->pSvc->Release();
		this->pLoc->Release();
		if(this->pEnumerator != 0)
			this->pEnumerator->Release();
		CoUninitialize();
	}
}

HRESULT WMIProcessList::init() {
	HRESULT lastResult;
	if (this->isServerConnected == 1) return 0;

	lastResult = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(lastResult)) {
		return 1;
	}
	lastResult = CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE,
		NULL
	);
	if (FAILED(lastResult)) {
		return 2;
	}
	lastResult = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&this->pLoc);
	if (FAILED(lastResult)) {
		return 3;
	}
	lastResult = this->pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		NULL,
		0,
		NULL,
		NULL,
		&this->pSvc
	);
	if (FAILED(lastResult)) {
		this->pLoc->Release();
		CoUninitialize();
		return 4;
	}
	this->isServerConnected = 1;
	return 0;
}

DWORD WMIProcessList::prosQueryLookupAll() {
	DWORD returnValue = 0;
	HRESULT lastResult;
	this->pEnumerator = 0;

	lastResult = pSvc->ExecQuery(bstr_t("WQL"),
		bstr_t("SELECT ProcessId,ExecutablePath,CommandLine,KernelModeTime,UserModeTime FROM Win32_Process"),
		WBEM_FLAG_FORWARD_ONLY,
		NULL,
		&this->pEnumerator
	);

	if (FAILED(lastResult))
		returnValue = 1; //Failed
	else
		returnValue = 0; //Success

	return returnValue;
}

ULONGLONG WMIProcessList::cvtBSTR_ulong(LPWSTR num) {
	unsigned int len = SysStringLen(num);
	ULONGLONG cvtNum = 0;
	for (unsigned int i = 0; i < len; i++) {
		if (num[i] >= L'0' && num[i] <= L'9')
			cvtNum = cvtNum * 10 + (num[i] - L'0');
		else {
			cvtNum = 0;
			break;
		}
	}
	return cvtNum;
}

DWORD WMIProcessList::getQueryDataAll(ProcessMap* pMap) {
	IWbemClassObject* pclsObj = 0;
	VARIANT ProcessId, ExecutablePath, CommandLine, KernelModeTime, UserModeTime;
	ULONG returnVal = 0, len1 = 0, len2 = 0;
	HRESULT hr;
	LPWSTR wChar1 = 0;
	LPWSTR wChar2 = 0;
	ProcessProp pProp;
	if (prosQueryLookupAll() != 0) {
		std::wcerr << "Failed to fetch process list.\n";
		return 1;
	}

	while (this->pEnumerator) {
		hr = this->pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &returnVal);
		if (returnVal == 0) break;
		VariantInit(&ProcessId);
		VariantInit(&ExecutablePath);
		VariantInit(&CommandLine);
		VariantInit(&KernelModeTime);
		VariantInit(&UserModeTime);
		hr = pclsObj->Get(L"ProcessId", 0, &ProcessId, 0, 0);
		hr = pclsObj->Get(L"ExecutablePath", 0, &ExecutablePath, 0, 0);
		hr = pclsObj->Get(L"CommandLine", 0, &CommandLine, 0, 0);
		hr = pclsObj->Get(L"KernelModeTime", 0, &KernelModeTime, 0, 0);
		hr = pclsObj->Get(L"UserModeTime", 0, &UserModeTime, 0, 0);

		if (ProcessId.uintVal == 0) continue;

		pProp.pid = ProcessId.uintVal;
		pProp.procKernel = cvtBSTR_ulong(KernelModeTime.bstrVal);
		pProp.procUser = cvtBSTR_ulong(UserModeTime.bstrVal);
		if (!pMap->contains(pProp.pid)) {
			copyStringBSTR(&pProp.exePath, ExecutablePath.bstrVal);
			copyStringBSTR(&pProp.commandLine, CommandLine.bstrVal);
			pMap->addMemoryWChar(pProp.exePath);
			pMap->addMemoryWChar(pProp.commandLine);
			if (pMap->add(&pProp) != 0) {
				returnVal = 1; //Failed
				break;
			}
		}
		else {
			pMap->updateProp(&pProp);
		}
		pProp.pid = 0;
		pProp.procKernel = 0;
		pProp.procUser = 0;
		pProp.commandLine = 0;
		pProp.exePath = 0;
		wChar1 = 0;
		wChar2 = 0;
		VariantClear(&ProcessId);
		VariantClear(&ExecutablePath);
		VariantClear(&CommandLine);
		VariantClear(&KernelModeTime);
		VariantClear(&UserModeTime);
		pclsObj->Release();
	}
	this->pEnumerator->Release();
	return returnVal;
}

INT WMIProcessList::wmiListInit(WMIListPECKU &wmiList) {
	if (this->wmiListFlag != 0)
		return -1;  // Already running

	if (prosQueryLookupAll() != 0) {
		std::wcerr << "Failed to fetch process list.\n";
		return 1;  // Failed
	}
	this->wmiListFlag = 1;
	return 0;
}

INT WMIProcessList::wmiListNext(WMIListPECKU &wmiList) {
	IWbemClassObject* pclsObj = 0;
	INT returnVal = 0;

	if (this->wmiListFlag == 1) {
		if (this->pEnumerator) {
			this->pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &(wmiList.ret));
			if (wmiList.ret != 0) {
				VariantInit(&wmiList.ProcessId);
				VariantInit(&wmiList.ExecutablePath);
				VariantInit(&wmiList.CommandLine);
				VariantInit(&wmiList.KernelModeTime);
				VariantInit(&wmiList.UserModeTime);
				pclsObj->Get(L"ProcessId", 0, &wmiList.ProcessId, 0, 0);
				pclsObj->Get(L"ExecutablePath", 0, &wmiList.ExecutablePath, 0, 0);
				pclsObj->Get(L"CommandLine", 0, &wmiList.CommandLine, 0, 0);
				pclsObj->Get(L"KernelModeTime", 0, &wmiList.KernelModeTime, 0, 0);
				pclsObj->Get(L"UserModeTime", 0, &wmiList.UserModeTime, 0, 0);
				pclsObj->Release();
				returnVal = 1;
			}
			else {
				this->pEnumerator->Release();
				this->pEnumerator = 0;
				this->wmiListFlag = 0;
				returnVal = 0;


				#ifdef __DEBUG_
					std::wcout << "[wmiListNext] - Loop ended\n";
				#endif // __DEBUG_


			}
		}
	}
#ifdef __DEBUG_
	else {
		std::wcout << "[wmiListNext] - Failed to iterate, u should call wmiListInit before calling wmiListNext\n";
	}
#endif // __DEBUG_

	
	return returnVal;
}

ListResultWMI::ListResultWMI(): sysKernelTime(0), sysUserTime(0),
							c_sysKernelTime(0), c_sysUserTime(0), topEntries(0),
							maxEntries(0), totalProcess(0), updateId(0) { }
ListResultWMI::~ListResultWMI() { }

DWORD ListResultWMI::init() {
	//init WMI
	if (this->wmiList.init() != 0) {
		return 1; // WMI init Error
	}

	// init process list
	if (this->wmiList.getQueryDataAll(&this->mainlist) == 1) {
		return 2; // WMI failed
	}

	// set system time
	if (setTimes(&this->sysKernelTime, &this->sysUserTime) == 1) {
		return 3; // System time failed
	}

	return 0;
}

VOID ListResultWMI::setEntries(DWORD entries) {
	this->maxEntries = entries;
}

bool ListResultWMI::Result::operator<(const ListResultWMI::Result& res) const {
	if (this->cpuUsage < res.cpuUsage)
		return true;
	else
		return false;
}

VOID ListResultWMI::updateTotalProcess() {
	if (totalProcess < this->mainlist.size()) {
		totalProcess = (DWORD)this->mainlist.size();
	}
}

DWORD ListResultWMI::setTimes(PULONGLONG kernel, PULONGLONG user) {
	FILETIME sysKernelF, sysUserF;
	ULARGE_INTEGER ulint;

	if (GetSystemTimes(0, &sysKernelF, &sysUserF) == 0) {

#ifdef __DEBUG_
		std::wcout << "[setTimes] - failed to get system time\n";
#endif // __DEBUG_

		return 1;
	}

	ulint.LowPart = sysKernelF.dwLowDateTime;
	ulint.HighPart = sysKernelF.dwHighDateTime;
	*kernel = ulint.QuadPart;

	ulint.QuadPart = (ULONGLONG)0;

	ulint.LowPart = sysUserF.dwLowDateTime;
	ulint.HighPart = sysUserF.dwHighDateTime;
	*user = ulint.QuadPart;

	return 0;
}

VOID ListResultWMI::forceAddResult(DWORD pid, DWORD cpu) {

	if (this->topResults2.size() < this->maxEntries) {
		this->topResults2[cpu].push_back(pid);
		return;
	}

	auto isPresent = this->topResults2.find(cpu);
	if (isPresent == this->topResults2.end()) {
		isPresent = this->topResults2.begin();
		isPresent->second.clear();
		this->topResults2.erase(isPresent);
		this->topResults2[cpu].push_back(pid);
	}
	else {
		isPresent->second.push_back(pid);
	}
}

VOID ListResultWMI::updateResult2(DWORD pid, DWORD cpu) {

	auto begin = this->topResults2.begin();
	auto end = this->topResults2.end();
	std::vector<DWORD>::iterator pidItr;

	if (cpu == 0)
		return;

	// travers all pid
	while (begin != end) {
		auto  pidlist = begin->second.begin();
		auto pidlist_end = begin->second.end();
		while (pidlist != pidlist_end) {
			if (pid == *pidlist)
				break;
			pidlist++;
		}

		if (pidlist != pidlist_end) {
			pidItr = pidlist;
			break;
		}
		begin++;
	}

	// add entry
	if (end == begin) {
		forceAddResult(pid, cpu);
	}
	else if (cpu > begin->first) {
		begin->second.erase(pidItr);
		if (begin->second.size() == 0) {
			this->topResults2.erase(begin);
			this->topResults2[cpu].push_back(pid);
		}
		else {
			forceAddResult(pid, cpu);
		}
	}

	return;
}

DWORD ListResultWMI::update_pick() {
	ULONGLONG sysKernelDiff, sysUserDiff;
	ULONGLONG procKernelDiff, procUserDiff;
	ProcessProp pProp1, pProp2;
	DWORD value, returnVal;
	WMIListPECKU wmiList;

	if (setTimes(&this->c_sysKernelTime, &this->c_sysUserTime) != 0)
		return 1; // Failed


	returnVal = 0;
	sysKernelDiff = this->c_sysKernelTime - this->sysKernelTime;
	sysUserDiff = this->c_sysUserTime - this->sysUserTime;

	this->sysKernelTime = this->c_sysKernelTime;
	this->sysUserTime = this->c_sysUserTime;

	this->updateId += 1;
	if (this->wmiList.wmiListInit(wmiList) == 0) {
		while (this->wmiList.wmiListNext(wmiList)) {
			if (wmiList.ProcessId.uintVal != 0) {
				pProp2.pid = wmiList.ProcessId.uintVal;
				pProp2.procKernel = this->wmiList.cvtBSTR_ulong(wmiList.KernelModeTime.bstrVal);
				pProp2.procUser = this->wmiList.cvtBSTR_ulong(wmiList.UserModeTime.bstrVal);
				pProp2.metaData = this->updateId;
				if (this->mainlist.contains(pProp2.pid)) {
					pProp1.pid = pProp2.pid;
					this->mainlist.getProp(&pProp1);
					if (pProp1.pid == 0) {

#ifdef __DEBUG_
						std::wcout << "[update_pick] - failed at getProp - no such pid present in main list\n";
#endif // __DEBUG_

						returnVal = 1;
						break;
					}
					procKernelDiff = pProp2.procKernel - pProp1.procKernel;
					procUserDiff = pProp2.procUser - pProp1.procUser;
					value = 100.0 * (double)((procKernelDiff + procUserDiff) / (double)(sysKernelDiff + sysUserDiff));
					if(value != 0)
						updateResult2(pProp2.pid, value);
					if (this->mainlist.updateProp(&pProp2) != 0) {

#ifdef __DEBUG_
						std::wcout << "[update_pick] - failed at updateProp - no such pid present in main list\n";
#endif // __DEBUG_


						returnVal = 1;
						break;
					}
					pProp2.pid = 0;
					pProp2.procKernel = 0;
					pProp2.procUser = 0;
				}
				else {

#ifdef __DEBUG_
					std::wcout << "[update_pick] - found new entry while updating process list\b";
					if (pProp2.exePath != 0)
						std::wcout << "[update_pick] - process name : " << pProp2.exePath << std::endl;
					else
						std::wcout << "[update_pick] - PID : " << pProp2.pid << std::endl;
#endif // __DEBUG_

					// add pProp
					copyStringBSTR(&pProp2.commandLine, wmiList.CommandLine.bstrVal);
					copyStringBSTR(&pProp2.exePath, wmiList.ExecutablePath.bstrVal);
					this->mainlist.addMemoryWChar(pProp2.commandLine);
					this->mainlist.addMemoryWChar(pProp2.exePath);
					this->mainlist.add(&pProp2);
				}
			}

			VariantClear(&wmiList.CommandLine);
			VariantClear(&wmiList.ExecutablePath);
			VariantClear(&wmiList.KernelModeTime);
			VariantClear(&wmiList.UserModeTime);
			VariantClear(&wmiList.ProcessId);
		}
	}
	else {

#ifdef __DEBUG_
		std::wcout << "[update_pick] - failed to init wmiList\n";
#endif // __DEBUG_

		returnVal = 1; // failed
	}

	return returnVal;
}

VOID ListResultWMI::printResult() {
	ProcessProp pp;
	auto itr = this->topResults2.begin();
	auto end_itr = this->topResults2.end();
	std::wcout << "\n====================================";
	while (itr != end_itr) {
		std::wcout << "\nCPU : " << itr->first;
		std::wcout << "%\n---------";
		auto pidItr = itr->second.begin();
		auto pidItrEnd = itr->second.end();
		while (pidItr != pidItrEnd) {
			pp.pid = *pidItr;
			this->mainlist.getAllProp(&pp);
			std::wcout << "\nPid  : " << pp.pid;
			if(pp.exePath != 0)
				std::wcout << "\nPath : " << pp.exePath;
			else
				std::wcout << "\nPath : <Access Denied>";
			if(pp.commandLine != 0)
				std::wcout << "\nCmd   : " << pp.commandLine;
			else
				std::wcout << "\nCmd  : <Access Denied>";
			std::wcout << std::endl;
			pidItr++;
		}
		std::wcout << std::endl;
		itr++;
	}
}