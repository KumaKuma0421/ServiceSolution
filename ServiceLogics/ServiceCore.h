//
// ServiceCore.h
//

#pragma once

#define EVENT_STOP _T("SERVICE_STOP_EVENT")

#include "ServiceCommon.hpp"
#include "Event.hpp"
#include "EventLogger.h"

typedef VOID (WINAPI *ServiceMain)(DWORD dwArgc, LPTSTR* lptszArgv);
typedef VOID (WINAPI *ControlHandler)(DWORD dwControlCode);

class DECLSPEC ServiceCore final
{
public:
    ServiceCore(EventLogger& logger);
    ~ServiceCore();

    BOOL WINAPI Entry(
        ServiceMain fnServiceMain,
        ControlHandler fnHandler);
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

    ControlHandler _fnHandler;
};

