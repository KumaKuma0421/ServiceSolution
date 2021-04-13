//
// EventLogger.h
//

#pragma once

#include "ServiceMessage.h"

class EventLogger final
{
public:
    EventLogger()
    {
        _hEventSource = nullptr;
    };

    ~EventLogger()
    {
        if (_hEventSource != nullptr)
            ::DeregisterEventSource(_hEventSource);
    };

    BOOL Init(LPCTSTR lpctszServiceName);
    BOOL Exit();

    BOOL Log(
        WORD wEventType,
        WORD wEventCategory,
        DWORD dwEventID,
        WORD wArgc,
        ...);

    BOOL ApiError(
        WORD wCategory,
        DWORD dwErrorCode,
        LPCTSTR lpctszFunctionName,
        LPCTSTR lpctszErrorAPI);

    BOOL TraceStart(WORD wCategory, LPCTSTR lpctszFunctionName);
    BOOL TraceFinish(WORD wCategory, LPCTSTR lpctszFunctionName);
    BOOL Trace(WORD wCategory, LPCTSTR lpctszFunctionName, LPCTSTR lpctszMessage = nullptr);

private:
    HANDLE _hEventSource;
};

