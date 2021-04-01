//
// Thread.h
//

#pragma once

#include "Event.hpp"

class Thread
{
public:
    Thread();
    virtual ~Thread();

    virtual BOOL Start(LPCTSTR lpctszStopEvent);
    virtual BOOL Stop();
    virtual BOOL Suspend();
    virtual BOOL Resume();

    virtual BOOL Run() = 0;

protected:
    Event _event;

private:
    static DWORD ThreadFunction(LPVOID lpvParam);

    HANDLE _hThread;
    DWORD _dwThread;
};