//
// EventViewer.h
//

#pragma once

class EventViewer
{
public:
    EventViewer();
    ~EventViewer();

    BOOL Init(LPCTSTR lpctszServiceName);
    BOOL Exit();

    BOOL Log(
        WORD wEventType,
        WORD wEventCategory,
        DWORD dwEventID,
        WORD wArgc,
        LPCTSTR* lpctszArgs);

    BOOL Open(LPCTSTR lpctszServiceName);
    BOOL Close();

    BOOL Read(
        DWORD dwReadFlags,
        DWORD dwRecordOffset,
        LPVOID lpvBuffer,
        DWORD& dwBufSize);

    BOOL Clear();

private:
    HANDLE _hEventSource;
};

