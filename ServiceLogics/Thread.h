//
// Thread.h
//

#pragma once

#include "Event.hpp"

class DECLSPEC Thread
{
public:
    Thread();
    virtual ~Thread();

    virtual BOOL Start();
    virtual BOOL Stop();
    virtual BOOL Suspend();
    virtual BOOL Resume();

    virtual BOOL Run() = 0;
    virtual BOOL Wait(DWORD dwMillisec = INFINITE);

protected:
    Event _event;

private:
    static DWORD ThreadFunction(LPVOID lpvParam);

    HANDLE _hThread;
    DWORD _dwThread;
};