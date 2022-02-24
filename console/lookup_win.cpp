#include <iostream>
#include <vector>
#include "arguments.h"
#include "../main/process_lookup.h"
#include "../main/process_block.h"

DWORD _runPL(DWORD totalTime, DWORD timeGranularity, DWORD nEntries) {
	DWORD exitCode = 0;
	ULONGLONG tickCountTotal, tickCountGran, tickCountCurr;
	DWORD tickValue1, tickValue2;
	ListResultWMI listResult;
	ProcessProp pProperties;
	if (timeGranularity > totalTime) {
		std::wcerr << "[Error] Invalid parameter!\nTotal time  : " << totalTime;
		std::wcerr << " min\nTime granularity : " << timeGranularity;
		std::wcerr << " min\nTime granularity should not be greater than Total time.\n";
		exitCode = 1;
		goto exitout;
	}
	if (timeGranularity >= 46080 || totalTime >= 46080) {
		std::wcerr << "[Exception] \nAs of current version. Specified time should not be greater than 46080 min\n";
		exitCode = 1;
		goto exitout;
	}
	if (timeGranularity == 0 || totalTime == 0 || nEntries == 0) {
		std::wcerr << "[Error] Invalid argument values!\n";
		exitCode = 1;
		goto exitout;
	}

	listResult.setEntries(nEntries);

	// In milliseconds
	totalTime = totalTime * 60 * 1000;
	timeGranularity = timeGranularity * 60 * 1000;

	if (listResult.init() > 0) {
		std::wcerr << "[Error] Unable to initiate ProcessLookup.\n";
		std::wcerr << "        - check if Windows Management Instrumentation service is running.\n";
		std::wcerr << "        - run following command to start WMI service: 'sc start Winmgmt'\n";
		exitCode = 1;
		goto exitout;
	}

	// milliseconds
	tickCountTotal = GetTickCount64();
	tickCountGran = tickCountTotal;
	while (true) {
		tickCountCurr = GetTickCount64();
		tickValue1 = tickCountCurr - tickCountTotal;
		tickValue2 = tickCountCurr - tickCountGran;
		if (tickValue1 >= totalTime) break;
		if (tickValue2 >= timeGranularity) {
			tickCountGran = tickCountCurr;
		}
		if (listResult.update_pick() != 0) {
			std::wcerr << "[Error] while updating list. Ignoring this round...\n";
		}
		Sleep(1000); // Normal mode
	}
	listResult.printResult();

exitout:
	return exitCode;
}


int main(int argc, char** argv) {
	ArgumentParser aParser;
	DWORD totalTime, timeGranularity, nEntries;
	int returnVal = 0;

	returnVal = aParser.parseAg(argc, argv);
	if (returnVal == 1) {
		std::wcerr << "[Error] Invalid Argument!\n";
		return 1;
	}
	else if (returnVal == 2) {
		return 0;
	}

	aParser.getExecutionParam(&totalTime, &timeGranularity, &nEntries);

	std::wcout << "Total Time       : " << totalTime;
	std::wcout << " min\nTime Granularity : " << timeGranularity;
	std::wcout << " min\nNo of Entries    : " << nEntries;

	std::wcout << "\n";
	returnVal = _runPL(totalTime, timeGranularity, nEntries);

	if (returnVal == 0)
		std::wcout << "\n****Program executed successfully****\n";
	else
		std::wcerr << "\n[[[Program exited with unexpected error]]]\n";

	return returnVal;
}

