//
// Thread.h
//

#pragma once

#define STOP_EVENT _T("STOP_MY_SERVICE_EVENT")

#include "Event.hpp"
#include "EventLogger.h"
#include "ServiceMessage.h"

class DECLSPEC Thread
{
public:
    Thread(EventLogger& logger);
    virtual ~Thread();

    virtual BOOL Start(LPCTSTR lpctszStopEvent);
    virtual BOOL Stop();
    virtual BOOL Suspend();
    virtual BOOL Resume();

    virtual BOOL Run() = 0;
    virtual BOOL Wait(DWORD dwMillisec = INFINITE);

protected:
    Event _event;
    EventLogger& _logger;

private:
    Thread();
    static DWORD ThreadFunction(LPVOID lpvParam);

    HANDLE _hThread;
    DWORD _dwThread;
};