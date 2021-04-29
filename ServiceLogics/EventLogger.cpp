//! @file   EventLogger.cpp
//! @brief  イベントログに書き込むクラス
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

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

BOOL EventLogger::ApiError(
    WORD wCategory,
    DWORD dwErrorCode,
    LPCTSTR lpctszFunctionName,
    LPCTSTR lpctszErrorAPI)
{
    DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_SYSTEM;
    LPCVOID lpcvSource = nullptr;
    DWORD dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
    LPTSTR lptszBuffer = nullptr;
    DWORD dwSize = 0;
    va_list arguments = nullptr;

    DWORD dwRet = ::FormatMessage(
        dwFlags,
        lpcvSource,
        dwErrorCode,
        dwLanguageId,
        (LPTSTR)&lptszBuffer,
        dwSize,
        &arguments);

    BOOL ret = Log(
        EVENTLOG_ERROR_TYPE,
        wCategory,
        SVC_ERROR_API,
        3,
        lpctszFunctionName,
        lpctszErrorAPI,
        lptszBuffer);

    ::LocalFree(lptszBuffer);

    return dwRet == 0 ? FALSE : TRUE;
}

BOOL EventLogger::TraceStart(WORD wCategory, LPCTSTR lpctszFunctionName)
{
#ifdef _DEBUG
    return Log(
        EVENTLOG_INFORMATION_TYPE,
        wCategory,
        SVC_INFO_TRACE,
        2,
        lpctszFunctionName,
        _T("Start"));
#else
    return TRUE;
#endif
}

BOOL EventLogger::TraceFinish(WORD wCategory, LPCTSTR lpctszFunctionName)
{
#ifdef _DEBUG
    return Log(
        EVENTLOG_INFORMATION_TYPE,
        wCategory,
        SVC_INFO_TRACE,
        2,
        lpctszFunctionName,
        _T("Finish"));
#else
    return TRUE;
#endif
}

BOOL EventLogger::Trace(
    WORD wCategory,
    LPCTSTR lpctszFunctionName,
    LPCTSTR lpctszMessage)
{
#ifdef _DEBUG
    return Log(
        EVENTLOG_INFORMATION_TYPE,
        wCategory,
        SVC_INFO_TRACE,
        2,
        lpctszFunctionName,
        lpctszMessage);
#else
    return TRUE;
#endif
}
