//
// EventLogger.h
//

#pragma once

#include "ServiceMessage.h"

class DECLSPEC EventLogger final
{
public:
    EventLogger()
    {
        _hEventSource = nullptr;
    };

    ~EventLogger()
    {
        if (_hEventSource != nullptr) Exit();
    };

    BOOL WINAPI Init(LPCTSTR lpctszServiceName);
    BOOL WINAPI Exit();

    BOOL WINAPI Log(
        WORD wEventType,
        WORD wEventCategory,
        DWORD dwEventID,
        WORD wArgc,
        ...);

    BOOL WINAPI ApiError(
        WORD wCategory,
        DWORD dwErrorCode,
        LPCTSTR lpctszFunctionName,
        LPCTSTR lpctszErrorAPI);

    BOOL WINAPI TraceStart(WORD wCategory, LPCTSTR lpctszFunctionName);
    BOOL WINAPI TraceFinish(WORD wCategory, LPCTSTR lpctszFunctionName);
    BOOL WINAPI Trace(
        WORD wCategory,
        LPCTSTR lpctszFunctionName,
        LPCTSTR lpctszMessage = nullptr);

private:
    HANDLE _hEventSource;
};

