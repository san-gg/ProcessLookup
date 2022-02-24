#include <iostream>
#include "process_map.h"

ProcessMap::ProcessMap() : pProp(0) { }
ProcessMap::~ProcessMap() {
	for (auto& mem : this->extraMemory) {
		delete[] (LPWSTR)(mem);
	}
	this->extraMemory.clear();

	for (auto& mem2 : this->procMap) {
		delete mem2.second;
	}
	this->procMap.clear();
}

ULONG64 ProcessMap::size() {
	return this->procMap.size();
}

//LPWSTR ProcessMap::getMemoryWchar(DWORD len) {
//	LPWSTR ptr = new (std::nothrow) WCHAR[len];
//	if (ptr != 0)
//		this->extraMemory.push_back(ptr);
//	return ptr;
//}

VOID ProcessMap::addMemoryWChar(LPWSTR mem) {
	if (mem != 0)
		this->extraMemory.push_back(mem);
	return;
}

DWORD ProcessMap::add(ProcessProp* pProp) {
	ProcessProp* ptr = new (std::nothrow) ProcessProp();
	DWORD returnVal = 0;
	if (ptr != 0) {
		ptr->pid = pProp->pid;
		ptr->procKernel = pProp->procKernel;
		ptr->procUser = pProp->procUser;
		ptr->commandLine = pProp->commandLine;
		ptr->exePath = pProp->exePath;
		this->procMap.insert({ ptr->pid, ptr });
	}
	else {
		returnVal = 1;
	}
	return returnVal;
}

DWORD ProcessMap::contains(DWORD pid) {
	DWORD returnVal;
	returnVal = (this->procMap.find(pid) != this->procMap.end());
	return returnVal;
}

VOID ProcessMap::getProp(ProcessProp* pProp) {
	ProcessProp* e;
	if (contains(pProp->pid)) {
		e = this->procMap[pProp->pid];
		pProp->procKernel = e->procKernel;
		pProp->procUser = e->procUser;
	}
	else {
		pProp->commandLine = 0;
		pProp->pid = 0;
		pProp->procKernel = 0;
		pProp->procUser = 0;
		pProp->exePath = 0;
	}
}

VOID ProcessMap::getAllProp(ProcessProp* pProp) {
	ProcessProp* e;
	if (contains(pProp->pid)) {
		e = this->procMap[pProp->pid];
		pProp->procKernel = e->procKernel;
		pProp->procUser = e->procUser;
		pProp->commandLine = e->commandLine;
		pProp->pid = e->pid;
		pProp->procKernel = e->procKernel;
		pProp->procUser = e->procUser;
		pProp->exePath = e->exePath;
		pProp->metaData = e->metaData;
	}
	else {
		pProp->commandLine = 0;
		pProp->pid = 0;
		pProp->procKernel = 0;
		pProp->procUser = 0;
		pProp->exePath = 0;
		pProp->metaData = 0;
	}
}

DWORD ProcessMap::updateProp(ProcessProp* pProp) {
	ProcessProp* e;
	if (contains(pProp->pid)) {
		e = this->procMap[pProp->pid];
		e->procKernel = pProp->procKernel;
		e->procUser = pProp->procUser;
		e->metaData = pProp->metaData;
		return 0;
	}
	return 1;
}

VOID ProcessMap::begin() {
	this->itr = this->procMap.begin();
}

DWORD ProcessMap::next() {
	this->itr++;
	if (this->itr != this->procMap.end())
		return 1;
	else
		return 0;
}

// No command line and exe path required
VOID ProcessMap::getItrData(ProcessProp* pProp) {
	ProcessProp* d = this->itr->second;
	pProp->pid = d->pid;
	pProp->procKernel = d->procKernel;
	pProp->procUser = d->procUser;
}

//VOID ProcessMap::itrRemove() {
//	delete itr->second;
//	this->itr = this->procMap.erase(this->itr);
//	this->itr--;
//}
