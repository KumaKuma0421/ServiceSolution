//
// Event.hpp
//

#pragma once

#include "pch.h"

class Event final
{
public:
    Event()
    {
        _hEvent = nullptr;
    };

    ~Event()
    {
        if (_hEvent == nullptr) Close();
    }

    BOOL Create(LPCTSTR lpctszEventName)
    {
        _hEvent = ::CreateEvent(
            nullptr,
            TRUE,  // manual reset event
            FALSE, // not signaled
            lpctszEventName);

        return _hEvent == nullptr ? FALSE : TRUE;
    };

    BOOL Open(LPCTSTR lpctszEventName)
    {
        _hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpctszEventName);

        return _hEvent == nullptr ? FALSE : TRUE;
    };

    BOOL Set()
    {
        return ::SetEvent(_hEvent);
    };

    BOOL Reset()
    {
        return ::ResetEvent(_hEvent);
    };

    DWORD Wait(DWORD dwMilliSeconds = INFINITE)
    {
        return ::WaitForSingleObject(_hEvent, dwMilliSeconds);
    };

    BOOL Close()
    {
        return ::CloseHandle(_hEvent);
    };

private:
    HANDLE _hEvent;
};
