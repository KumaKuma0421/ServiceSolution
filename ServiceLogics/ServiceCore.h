//
// ServiceCore.h
//

#pragma once

#define EVENT_STOP _T("SERVICE_STOP_EVENT")

#include "ServiceCommon.hpp"
#include "Event.hpp"
#include "EventLogger.h"

class DECLSPEC ServiceCore final
{
public:
    ServiceCore(EventLogger& logger);
    ~ServiceCore();

    BOOL WINAPI Entry(
        LPSERVICE_MAIN_FUNCTION fnServiceMain,
        LPHANDLER_FUNCTION fnHandler);
    VOID WINAPI Main(DWORD dwArgc, LPTSTR* lptszArgv);
    VOID WINAPI Handler(DWORD dwControlCode);

private:
    BOOL WINAPI Init();
    BOOL WINAPI Start();
    BOOL WINAPI Stop();
    BOOL WINAPI Suspend();
    BOOL WINAPI Resume();
    BOOL WINAPI Exit();

    BOOL WINAPI ReportStatus(
        DWORD dwCurrentState,
        DWORD dwWin32ExitCode = NO_ERROR,
        DWORD dwWaitHint = MAX_STATE_TRANSITION_TIME);

    DWORD _dwControlCode;

    ServiceStatusHandler _handler;
    ServiceStatus _status;

    Event _event;
    EventLogger& _logger;

    LPHANDLER_FUNCTION _fnHandler;
};

