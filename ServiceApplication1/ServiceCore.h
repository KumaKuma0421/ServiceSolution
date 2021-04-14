//
// ServiceCore.h
//

#pragma once

#define EVENT_STOP _T("SERVICE_STOP_EVENT")

#include "Main.h"
#include "ServiceCommon.hpp"
#include "Event.hpp"
#include "EventLogger.h"

class ServiceCore final
{
public:
    ServiceCore(EventLogger& logger);
    ~ServiceCore();

    BOOL Entry();
    VOID Main(DWORD dwArgc, LPTSTR* lptszArgv);
    VOID Handler(DWORD dwControlCode);

private:
    BOOL Init();
    BOOL Start();
    BOOL Stop();
    BOOL Suspend();
    BOOL Resume();
    BOOL Exit();

    BOOL ReportStatus(
        DWORD dwCurrentState,
        DWORD dwWin32ExitCode = NO_ERROR,
        DWORD dwWaitHint = MAX_STATE_TRANSITION_TIME);

    DWORD _dwControlCode;

    ServiceStatusHandler _handler;
    ServiceStatus _status;

    Event _event;
    EventLogger& _logger;
};

