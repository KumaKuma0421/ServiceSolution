//
// EventLogger.cpp
//

#include "pch.h"
#include "EventLogger.h"

BOOL EventLogger::Init(LPCTSTR lpctszServiceName)
{
    _hEventSource = ::RegisterEventSource(nullptr, lpctszServiceName);

    return _hEventSource == nullptr ? FALSE : TRUE;
}

BOOL EventLogger::Exit()
{
    return ::DeregisterEventSource(_hEventSource);
}

BOOL EventLogger::Log(
    WORD wEventType,
    WORD wEventCategory,
    DWORD dwEventID,
    WORD wArgc,
    ...)
{
    BOOL ret = FALSE;

    const WORD cwMessageArgs = 3;
    LPCTSTR lpctszArgs[cwMessageArgs] = { _T(""),_T(""),_T("") };
    va_list args;
    va_start(args, wArgc);
    for (WORD i = 0; i < wArgc; i++)
    {
        lpctszArgs[i] = va_arg(args, LPCTSTR);
    }
    va_end(args);

    if (_hEventSource != nullptr)
    {
        ret = ::ReportEvent(
            _hEventSource,  // event log handle
            wEventType,     // event type
            wEventCategory, // event category
            dwEventID,      // event identifier
            nullptr,        // no security identifier
            cwMessageArgs,  // size of lpszStrings array
            0,              // no binary data
            lpctszArgs,     // array of strings
            nullptr         // no binary data
        );
    }

    return ret;
}

BOOL EventLogger::TraceStart(WORD wCategory, LPCTSTR lpctszFunctionName)
{
#ifdef _DEBUG
    return Log(EVENTLOG_INFORMATION_TYPE, wCategory, SVC_INFO_RUNTIME, 2, lpctszFunctionName, _T("Start"));
#else
    return TRUE;
#endif
}

BOOL EventLogger::TraceFinish(WORD wCategory, LPCTSTR lpctszFunctionName)
{
#ifdef _DEBUG
    return Log(EVENTLOG_INFORMATION_TYPE, wCategory, SVC_INFO_RUNTIME, 2, lpctszFunctionName, _T("Finish"));
#else
    return TRUE;
#endif
}