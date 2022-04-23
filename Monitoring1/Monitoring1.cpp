//
// Monitoring1.cpp
//

#include <Windows.h>
#include <tchar.h>
#include <Pdh.h>
#include <clocale>
#include <vector>

#include "PerformanceMonitor.h"

#pragma comment(lib, "pdh.lib")

void ShowCounter ();

int main ()
{
	_tsetlocale (LC_ALL, _T ("Japanese"));

	ShowCounter ();
}

void ShowCounter ()
{
	BOOL ret;
	PerformanceMonitor monitor;

	ret = monitor.Open ();
	if (!ret) return;

	std::vector<PerformanceCounter*> counters;

	LPCTSTR lpctszCounterNames[]{
		_T ("\\Processor(_Total)\\% Processor Time"),
		_T ("\\Processor(0)\\% Processor Time"),
		_T ("\\Processor(1)\\% Processor Time"),
		_T ("\\Processor(2)\\% Processor Time"),
		_T ("\\Processor(3)\\% Processor Time"),
		_T ("\\Processor(_Total)\\% User Time"),
		_T ("\\Processor(0)\\% User Time"),
		_T ("\\Processor(1)\\% User Time"),
		_T ("\\Processor(2)\\% User Time"),
		_T ("\\Processor(3)\\% User Time"),
		_T ("\\Processor(_Total)\\% Privileged Time"),
		_T ("\\Processor(0)\\% Privileged Time"),
		_T ("\\Processor(1)\\% Privileged Time"),
		_T ("\\Processor(2)\\% Privileged Time"),
		_T ("\\Processor(3)\\% Privileged Time"),
		_T ("\\Processor(_Total)\\Interrupts/sec"),
		_T ("\\Processor(0)\\Interrupts/sec"),
		_T ("\\Processor(1)\\Interrupts/sec"),
		_T ("\\Processor(2)\\Interrupts/sec"),
		_T ("\\Processor(3)\\Interrupts/sec"),
		_T ("\\System\\Processor Queue Length"),
		_T ("\\System\\Context Switches/sec"),
		_T ("\\Memory\\Available MBytes"),
		_T ("\\Memory\\Pages/sec"),
		_T ("\\Paging File(_Total)\\% Usage"),
		_T ("\\Memory\\% Committed Bytes In Use"),
		_T ("\\Memory\\Committed Bytes"),
		_T ("\\PhysicalDisk(_Total)\\% Disk Time"),
		_T ("\\PhysicalDisk(0 C:)\\% Disk Time"),
		_T ("\\PhysicalDisk(_Total)\\Current Disk Queue Length"),
		_T ("\\PhysicalDisk(0 C:)\\Current Disk Queue Length"),
		_T ("\\PhysicalDisk(_Total)\\Disk Writes/sec"),
		_T ("\\PhysicalDisk(0 C:)\\Disk Writes/sec"),
		_T ("\\Network Interface(Microsoft Hyper-V Network Adapter)\\Bytes Received/sec"),
		_T ("\\Network Interface(Microsoft Hyper-V Network Adapter)\\Bytes Sent/sec"),
		_T ("\\Network Interface(Microsoft Hyper-V Network Adapter)\\Bytes Total/sec"),
		_T ("\\Network Interface(Microsoft Hyper-V Network Adapter)\\Output Queue Length"),
	};

	int iMaxTitleLength = 0;
	for (auto item : lpctszCounterNames)
	{
		PerformanceCounter* pCounter;
		pCounter = monitor.CreateCounter (item);
		if (pCounter)
		{
			if (iMaxTitleLength < pCounter->GetTitleLength ())
				iMaxTitleLength = pCounter->GetTitleLength ();
			counters.push_back (pCounter);
		}
	}

	ret = monitor.Query ();

	HANDLE hFile = ::CreateFile (_T (".\\Performance.log"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
	TCHAR tszValue[160] = { 0 };
	COORD coord = { 0, 0 };

	for (int i = 0; i < 300000; i++)
	{
		::SetConsoleCursorPosition (
			GetStdHandle (STD_OUTPUT_HANDLE),
			coord);

		Sleep (1000);

		ret = monitor.Query ();
		if (!ret) return;

		for (auto item : counters)
		{
			ret = item->OutputString (iMaxTitleLength, tszValue, sizeof (tszValue) / sizeof (TCHAR));
			if (!ret) return;
			_tprintf (_T ("%s\n"), tszValue);

			ret = item->OutputFile (hFile);
		}

		DWORD dwWritten;
		::WriteFile (hFile, _T ("\r\n"), 4, &dwWritten, nullptr);
	}

	::CloseHandle (hFile);

	for (auto item : counters)
	{
		delete item;
	}

	ret = monitor.Close ();
	if (!ret) return;
}