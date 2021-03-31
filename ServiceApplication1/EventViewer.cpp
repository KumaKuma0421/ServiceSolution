//
// EventViewer.cpp
//

#include "pch.h"
#include "EventViewer.h"

BOOL EventViewer::Init(LPCTSTR lpctszServiceName)
{
	_lpctszServiceName = lpctszServiceName;
	_hEventSource = ::RegisterEventSource(nullptr, _lpctszServiceName);

	return _hEventSource == nullptr ? FALSE : TRUE;
}

BOOL EventViewer::Exit()
{
	return ::DeregisterEventSource(_hEventSource);
}

BOOL EventViewer::Write(
	WORD wEventType,
	WORD wEventCategory,
	DWORD dwEventID,
	WORD wArgc,
	LPCTSTR* lpctszArg)
{
	BOOL ret = FALSE;

	if (_hEventSource != nullptr)
	{
		ret = ::ReportEvent(
			_hEventSource,  // event log handle
			wEventType,     // event type
			wEventCategory, // event category
			dwEventID,      // event identifier
			nullptr,        // no security identifier
			wArgc,          // size of lpszStrings array
			0,              // no binary data
			lpctszArg,      // array of strings
			nullptr         // no binary data
		);
	}
	DWORD dwError = GetLastError();

	return ret;
}

BOOL EventViewer::TraceStart(
	WORD wCategory,
	LPCTSTR lpctszFunctionName)
{
#ifdef _DEBUG
	LPCTSTR lpctszArgs[3] = {
		lpctszFunctionName,
		_T("Start"),
		_T("")
	};

	return Write(
		EVENTLOG_INFORMATION_TYPE,
		wCategory,
		SVC_INFO_RUNTIME,
		3,
		(LPCTSTR*)&lpctszArgs);
#else
	return TRUE;
#endif
}

BOOL EventViewer::TraceFinish(
	WORD wCategory,
	LPCTSTR lpctszFunctionName)
{
#ifdef _DEBUG
	LPCTSTR lpctszArgs[3] = {
		lpctszFunctionName,
		_T("Finish"),
		_T("")
	};

	return Write(
		EVENTLOG_INFORMATION_TYPE,
		wCategory,
		SVC_INFO_RUNTIME,
		3,
		(LPCTSTR*)&lpctszArgs);
#else
	return TRUE;
#endif
}