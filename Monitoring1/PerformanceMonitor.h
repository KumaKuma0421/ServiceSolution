//
// PerformanceMonitor.h
//

#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Pdh.h>
#include <PdhMsg.h>
#include <winperf.h>

#pragma comment(lib, "pdh.lib")

class PerformanceCounter final
{
public:
	PerformanceCounter (HCOUNTER hCounter, PPDH_COUNTER_INFO pCounterInfo);
	~PerformanceCounter ();

	BOOL GetValues ();
	BOOL OutputString (int iMaxTitleLength, LPTSTR lptszValue, DWORD dwSize);
	BOOL OutputFile (HANDLE& hFile);
	int GetTitleLength () { return (int)_tcslen (_lpctszCounterName); };

private:
	VOID ParseInformation ();
	HCOUNTER _hCounter;
	PPDH_COUNTER_INFO _pCounterInfo;
	DWORD _dwType;
	LPCTSTR _lpctszCounterName;
	PDH_FMT_COUNTERVALUE _CurrentValue;
	PDH_FMT_COUNTERVALUE _Previous1Value;
	PDH_FMT_COUNTERVALUE _Previous2Value;
	PDH_FMT_COUNTERVALUE _Previous3Value;
	PDH_FMT_COUNTERVALUE _MaxValue;
	PDH_FMT_COUNTERVALUE _MinValue;
	LONGLONG _counter;
};

class PerformanceMonitor final
{
public:
	PerformanceMonitor ();
	~PerformanceMonitor ();

	BOOL Open ();
	BOOL Close ();
	PerformanceCounter* CreateCounter (LPCTSTR lpctszQueryString);
	BOOL Query ();

private:
	HQUERY _hQuery;
};