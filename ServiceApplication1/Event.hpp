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

    BOOL WINAPI Create(LPCTSTR lpctszEventName)
    {
        _hEvent = ::CreateEvent(
            nullptr,
            TRUE,  // manual reset event
            FALSE, // not signaled
            lpctszEventName);

        return _hEvent == nullptr ? FALSE : TRUE;
    };

    BOOL WINAPI Open(LPCTSTR lpctszEventName)
    {
        _hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpctszEventName);

        return _hEvent == nullptr ? FALSE : TRUE;
    };

    BOOL WINAPI Set()
    {
        return ::SetEvent(_hEvent);
    };

    BOOL WINAPI Reset()
    {
        return ::ResetEvent(_hEvent);
    };

    DWORD WINAPI Wait(DWORD dwMilliSeconds = INFINITE)
    {
        return ::WaitForSingleObject(_hEvent, dwMilliSeconds);
    };

    BOOL WINAPI Close()
    {
        return ::CloseHandle(_hEvent);
    };

private:
    HANDLE _hEvent;
};
