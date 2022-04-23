//
// PerformanceMonitor.cpp
//

#include "PerformanceMonitor.h"

PerformanceCounter::PerformanceCounter (
	HCOUNTER hCounter,
	PPDH_COUNTER_INFO pCounterInfo)
{
	_hCounter = hCounter;
	_pCounterInfo = pCounterInfo;
	ZeroMemory (&_CurrentValue, sizeof (_CurrentValue));
	ZeroMemory (&_Previous1Value, sizeof (_Previous1Value));
	ZeroMemory (&_MinValue, sizeof (_MinValue));
	ZeroMemory (&_MaxValue, sizeof (_MaxValue));
	_counter = 0;

	ParseInformation ();
}

PerformanceCounter::~PerformanceCounter ()
{
	if (_pCounterInfo) delete[] _pCounterInfo;
}

BOOL PerformanceCounter::GetValues ()
{
	_Previous3Value = _Previous2Value;
	_Previous2Value = _Previous1Value;
	_Previous1Value = _CurrentValue;

	PDH_STATUS status = ::PdhGetFormattedCounterValue (
		_hCounter,
		_dwType,
		NULL,
		&_CurrentValue);

	if (_counter > 1)
	{
		if (_dwType == PDH_FMT_DOUBLE)
		{
			if (_MaxValue.doubleValue < _CurrentValue.doubleValue) _MaxValue.doubleValue = _CurrentValue.doubleValue;
			if (_MinValue.doubleValue > _CurrentValue.doubleValue) _MinValue.doubleValue = _CurrentValue.doubleValue;
		}
		else
		{
			if (_dwType == PDH_FMT_LARGE)
			{
				if (_MaxValue.largeValue < _CurrentValue.largeValue) _MaxValue.largeValue = _CurrentValue.largeValue;
				if (_MinValue.largeValue > _CurrentValue.largeValue) _MinValue.largeValue = _CurrentValue.largeValue;
			}
			else
			{
				if (_MaxValue.longValue < _CurrentValue.longValue) _MaxValue.longValue = _CurrentValue.longValue;
				if (_MinValue.longValue > _CurrentValue.longValue) _MinValue.longValue = _CurrentValue.longValue;
			}
		}
	}
	else
	{
		if (_dwType == PDH_FMT_DOUBLE)
		{
			_MaxValue.doubleValue = _CurrentValue.doubleValue;
			_MinValue.doubleValue = _CurrentValue.doubleValue;
		}
		else
		{
			if (_dwType == PDH_FMT_LARGE)
			{
				_MaxValue.largeValue = _CurrentValue.largeValue;
				_MinValue.largeValue = _CurrentValue.largeValue;
			}
			else
			{
				_MaxValue.longValue = _CurrentValue.longValue;
				_MinValue.longValue = _CurrentValue.longValue;
			}
		}
	}

	_counter++;

	return status == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL PerformanceCounter::OutputString (int iMaxTitleLength, LPTSTR lptszValue, DWORD dwSize)
{
	BOOL ret;

	ret = GetValues ();
	if (ret)
	{
		if (_dwType == PDH_FMT_DOUBLE)
		{
			_stprintf_s (
				lptszValue,
				dwSize,
				_T ("%*s(%lld) : %6.3f%% Avg:%6.3f%% Min:%6.3f%% Max:%6.3f%% ƒ¢%6.3f%%"),
				iMaxTitleLength,
				_lpctszCounterName,
				_counter,
				_CurrentValue.doubleValue,
				((_Previous1Value.doubleValue * 5 + _Previous2Value.doubleValue * 3 + _Previous1Value.doubleValue * 2) / 10),
				_MinValue.doubleValue,
				_MaxValue.doubleValue,
				(_CurrentValue.doubleValue - _Previous1Value.doubleValue));
		}
		else
		{
			if (_dwType == PDH_FMT_LARGE)
			{
				_stprintf_s (
					lptszValue,
					dwSize,
					_T ("%*s(%lld) : %lld Avg:%lld Min:%lld Max:%lld ƒ¢%lld"),
					iMaxTitleLength,
					_lpctszCounterName,
					_counter,
					_CurrentValue.largeValue,
					((_Previous1Value.largeValue * 5 + _Previous2Value.largeValue * 3 + _Previous1Value.largeValue * 2) / 10),
					_MinValue.largeValue,
					_MaxValue.largeValue,
					(_CurrentValue.largeValue - _Previous1Value.largeValue));
			}
			else
			{
				_stprintf_s (
					lptszValue,
					dwSize,
					_T ("%*s(%lld) : %d Avg:%d Min:%d Max:%d ƒ¢%d"),
					iMaxTitleLength,
					_lpctszCounterName,
					_counter,
					_CurrentValue.longValue,
					((_Previous1Value.longValue * 5 + _Previous2Value.longValue * 3 + _Previous1Value.longValue * 2) / 10),
					_MinValue.longValue,
					_MaxValue.longValue,
					(_CurrentValue.longValue - _Previous1Value.longValue));
			}
		}
	}

	return ret;
}

BOOL PerformanceCounter::OutputFile (HANDLE& hFile)
{
	BOOL ret;
	TCHAR tszBuffer[256];
	DWORD dwSize = 256;

	ret = GetValues ();
	if (ret)
	{
		if (_dwType == PDH_FMT_DOUBLE)
		{
			_stprintf_s (
				tszBuffer,
				dwSize,
				_T ("%6.3f, %6.3f, %6.3f, %6.3f, %6.3f"),
				_CurrentValue.doubleValue,
				((_Previous1Value.doubleValue * 5 + _Previous2Value.doubleValue * 3 + _Previous1Value.doubleValue * 2) / 10),
				_MinValue.doubleValue,
				_MaxValue.doubleValue,
				(_CurrentValue.doubleValue - _Previous1Value.doubleValue));
		}
		else
		{
			if (_dwType == PDH_FMT_LARGE)
			{
				_stprintf_s (
					tszBuffer,
					dwSize,
					_T ("%lld, %lld, %lld, %lld, %lld"),
					_CurrentValue.largeValue,
					((_Previous1Value.largeValue * 5 + _Previous2Value.largeValue * 3 + _Previous1Value.largeValue * 2) / 10),
					_MinValue.largeValue,
					_MaxValue.largeValue,
					(_CurrentValue.largeValue - _Previous1Value.largeValue));
			}
			else
			{
				_stprintf_s (
					tszBuffer,
					dwSize,
					_T ("%d, %d, %d, %d, %d"),
					_CurrentValue.longValue,
					((_Previous1Value.longValue * 5 + _Previous2Value.longValue * 3 + _Previous1Value.longValue * 2) / 10),
					_MinValue.longValue,
					_MaxValue.longValue,
					(_CurrentValue.longValue - _Previous1Value.longValue));
			}
		}

		DWORD dwWritten;
		::WriteFile (hFile, tszBuffer, (DWORD)_tcslen (tszBuffer), &dwWritten, nullptr);
	}

	return ret;
}

VOID PerformanceCounter::ParseInformation ()
{
	_lpctszCounterName = _pCounterInfo->szFullPath;

	if (_pCounterInfo->dwType & PERF_DISPLAY_PERCENT)
	{
		_dwType = PDH_FMT_DOUBLE;
	}
	else
	{
		if (_pCounterInfo->dwType & PERF_SIZE_DWORD)
		{
			_dwType = PDH_FMT_LONG;
		}
		else
		{
			_dwType = PDH_FMT_LARGE;
		}
	}
}

// --------------------------------------------------------------------

PerformanceMonitor::PerformanceMonitor ()
{
	_hQuery = nullptr;
}

PerformanceMonitor::~PerformanceMonitor ()
{
	if (_hQuery) PdhCloseQuery (_hQuery);
}

BOOL PerformanceMonitor::Open ()
{
	PDH_STATUS status = ::PdhOpenQuery (nullptr, 0, &_hQuery);
	return status == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL PerformanceMonitor::Close ()
{
	PDH_STATUS status = ::PdhCloseQuery (_hQuery);
	return status == ERROR_SUCCESS ? TRUE : FALSE;
}

PerformanceCounter* PerformanceMonitor::CreateCounter (LPCTSTR lpctszQueryString)
{
	PerformanceCounter* response = nullptr;

	HCOUNTER hCounter;
	PDH_STATUS status = ::PdhAddCounter (
		_hQuery,
		lpctszQueryString,
		0,
		&hCounter);

	if (status == ERROR_SUCCESS)
	{
		DWORD dwSize = 0;
		status = ::PdhGetCounterInfo (hCounter, FALSE, &dwSize, nullptr);
		if (status == PDH_MORE_DATA)
		{
			PPDH_COUNTER_INFO pCounterInfo = (PPDH_COUNTER_INFO)new BYTE[dwSize];
			ZeroMemory (pCounterInfo, dwSize);
			status = ::PdhGetCounterInfo (hCounter, FALSE, &dwSize, pCounterInfo);
			if (status == ERROR_SUCCESS)
			{
				response = new PerformanceCounter (hCounter, pCounterInfo);
			}
		}
	}

	return response;
}

BOOL PerformanceMonitor::Query ()
{
	PDH_STATUS status = ::PdhCollectQueryData (_hQuery);
	return status == ERROR_SUCCESS ? TRUE : FALSE;
}