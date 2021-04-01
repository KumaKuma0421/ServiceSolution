//
// ServiceCore.h
//

#pragma once

#define SERVICE_NAME _T("ServiceApplication1")
#define SERVICE_DESCRIPTION _T("サンプルのサービス")

#define EVENT_STOP _T("SERVICE_STOP_EVENT")

#define COMMAND_INSTALL _T("/install")
#define COMMAND_REMOVE _T("/remove")

#define MAX_MESSAGE_LEN 256

#include "Main.h"
#include "Event.hpp"
#include "EventViewer.h"
#include "resource.h"

class ServiceHandler
{
public:
    ServiceHandler()
    {
        _handle = nullptr;
    };

    virtual ~ServiceHandler()
    {
        if (_handle != nullptr) Close();
    };

    virtual BOOL Close()
    {
        return ::CloseServiceHandle(_handle);
    }

    virtual SC_HANDLE GetHandle() { return _handle; };

protected:
    SC_HANDLE _handle;
};


class ServiceControlManager final : public ServiceHandler
{
public:
    BOOL Open()
    {
        _handle = ::OpenSCManager(
            nullptr,
            nullptr,
            SC_MANAGER_ALL_ACCESS);

        return _handle == nullptr ? FALSE : TRUE;
    };
};


class ServiceControl final : public ServiceHandler
{
public:
    ServiceControl(ServiceControlManager& manager, LPCTSTR lpctszServiceName)
        : _manager(manager)
    {
        _tcscpy_s(_tszServiceName, lpctszServiceName);
    };

    BOOL Create(LPCTSTR lpctszFilePath)
    {
        _handle = ::CreateService(
            _manager.GetHandle(),
            SERVICE_NAME,
            SERVICE_DESCRIPTION,
            SERVICE_ALL_ACCESS,
            SERVICE_WIN32_OWN_PROCESS,
            SERVICE_DEMAND_START,
            SERVICE_ERROR_NORMAL,
            lpctszFilePath,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr);

        return _handle == nullptr ? FALSE : TRUE;
    };

    BOOL Open()
    {
        _handle = ::OpenService(
            _manager.GetHandle(),
            _tszServiceName,
            DELETE);

        return _handle == nullptr ? FALSE : TRUE;
    };

    BOOL Delete()
    {
        return ::DeleteService(_handle);
    };

private:
    ServiceControl();

    ServiceControlManager& _manager;
    TCHAR _tszServiceName[MAX_PATH];
};


class ServiceCore final
{
public:
    ServiceCore();
    ~ServiceCore();

    BOOL Command(LPCTSTR lpctszCommand);

    BOOL Entry();
    BOOL Install();
    BOOL Remove();

    VOID Main(DWORD dwArgc, LPTSTR* lptszArgv);
    VOID Handler(DWORD dwControlCode);

private:
    BOOL Init();
    BOOL Exit();
    BOOL Start();
    BOOL Stop();
    BOOL Suspend();
    BOOL Resume();

    BOOL ReportStatus(
        DWORD dwCurrentState,
        DWORD dwWin32ExitCode,
        DWORD dwWaitHint);

    SERVICE_STATUS_HANDLE _hServiceStatus;
    SERVICE_STATUS _status;
    DWORD _dwControlCode;

    Event event;
    EventViewer _log;
};

