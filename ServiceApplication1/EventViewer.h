//
// EventViewer.h
//

#pragma once

#include "ServiceMessage.h"

class EventViewer final
{
public:
    EventViewer()
    {
        _hEventSource = nullptr;
        _lpctszServiceName = nullptr;
    };

    ~EventViewer()
    {
        if (_hEventSource != nullptr)
            ::DeregisterEventSource(_hEventSource);
    };

    BOOL Init(LPCTSTR lpctszServiceName);
    BOOL Exit();

    //! @ToDo lpctszArg�͔z��ł����Ǝv���B
    BOOL Write(
        WORD wEventType,
        WORD wEventCategory,
        DWORD dwEventID,
        WORD wArgc,
        LPCTSTR* lpctszArg);
    BOOL TraceStart(WORD wCategory, LPCTSTR lpctszFunctionName);
    BOOL TraceFinish(WORD wCategory, LPCTSTR lpctszFunctionName);


private:
    HANDLE _hEventSource;
    LPCTSTR _lpctszServiceName;
};

