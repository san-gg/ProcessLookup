#include <iostream>
#include "arguments.h"


ArgumentParser::ArgumentParser() : timeGranularity(1), totalTime(5), noOfEntries(5) { }

DWORD ArgumentParser::getTotalTime() {
	return totalTime;
}

DWORD ArgumentParser::getGranularity() {
	return timeGranularity;
}

DWORD ArgumentParser::strcompare(LPCSTR s1, LPCSTR s2) {
	DWORD x = 0;
	DWORD y = 0;
	DWORD cmp = 0;
	while (true) {
		if ((s1[x] == '\0') && (s2[y] == '\0')) break;
		if ((s1[x] == '\0') || (s2[y] == '\0')) {
			cmp = 1;
			break;
		}
		if (s1[x] != s2[y]) {
			cmp = 1;
			break;
		}
		x++;
		y++;
	}
	return cmp;
}

DWORD ArgumentParser::argvalue(LPCSTR actual, LPCSTR expected, LPCSTR value, LPDWORD store) {
	DWORD tmpVal = 0, returnVal = 0;
	this->ss.str("");
	this->ss.clear();
	if (value == 0)
		return 1;
	if (strcompare(actual, expected) == 0) {
		ss << value;
		ss >> tmpVal;
		*store = tmpVal;
	}
	if (tmpVal == 0) {
		returnVal = 1;
	}
	return returnVal;
}

DWORD ArgumentParser::argvalue(LPCSTR actual, LPCSTR expected, LPCSTR value) {
	this->ss.str("");
	this->ss.clear();
	if (strcompare(actual, expected) == 0)
		return 0;
	else
		return 1;
}

DWORD ArgumentParser::parseAg(DWORD argc, LPSTR* argv) {
	DWORD _argno = 1, time = -1, granularity = -1;
	LPSTR arg = 0;
	DWORD returnVal = 0, isArgCorrect = 0;
	std::stringstream ss;

	if (argc == 1) {
		std::wcout << "Setting Default Configuration\n";
		return 0;
	}

	while (_argno < argc) {
		arg = argv[_argno];
		if (argvalue(arg, "/t", argv[_argno + 1], &this->totalTime) == 0) {
			isArgCorrect = 1;
			_argno++;
		}
		if (argvalue(arg, "/tg", argv[_argno + 1], &this->timeGranularity) == 0) {
			isArgCorrect = 1;
			_argno++;
		}
		if (argvalue(arg, "/noe", argv[_argno + 1], &this->noOfEntries) == 0) {
			isArgCorrect = 1;
			_argno++;
		}
		if (argvalue(arg, "/?", argv[_argno + 1]) == 0) {
			isArgCorrect = 1;
			printHelp();
			returnVal = 2;
			break;
		}
		if (isArgCorrect == 0) {
			returnVal = 1;
			break;
		}
		_argno++;
		isArgCorrect = 0;
	}
	return returnVal;
}

VOID ArgumentParser::getExecutionParam(LPDWORD tTime, LPDWORD tGran, LPDWORD nEntry) {
	*tTime = this->totalTime;
	*tGran = this->timeGranularity;
	*nEntry = this->noOfEntries;
}

VOID ArgumentParser::printHelp() {
	std::wcout << "\nProcessLookup.exe [/t time] [/tg time_granularity] [/noe entries]\n\n";
	std::wcout << "Description:\n";
	std::wcout << "    This tool displays a list of all processes CPU usage.\n\n";
	std::wcout << "Parameter List:\n";
	std::wcout << "    /t     time                  Total time(in minutes) to monitor the processes.\n\n";
	std::wcout << "    /tg    time_granularity      Selected processes accounted for CPU usage after Time granularity(in minutes).\n\n";
	std::wcout << "    /noe   entries               Specifies no. of CPU usage to display.\n\n";
	std::wcout << "    /?     help                  Displays this help message.\n";
	return;
}