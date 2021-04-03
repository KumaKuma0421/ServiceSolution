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
#include "EventLogger.h"
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

class ServiceStatusHandler final
{
public:
    ServiceStatusHandler()
    {
        _hService = nullptr;
    };

    ~ServiceStatusHandler()
    {
        if (_hService != nullptr)
        {
            Exit();
        }
    }

    BOOL Init(LPCTSTR lpctszServiceName, LPHANDLER_FUNCTION lpfnCtrlHandler)
    {
        _hService = ::RegisterServiceCtrlHandler(lpctszServiceName, lpfnCtrlHandler);
        return _hService == nullptr ? FALSE : TRUE;
    };

    BOOL Exit()
    {
        BOOL ret = ::DeregisterEventSource(_hService);
        return ret;
    };

    SERVICE_STATUS_HANDLE GetHandle() { return _hService; };

private:
    SERVICE_STATUS_HANDLE _hService;
};

class ServiceStatus final
{
public:
    ServiceStatus() { ZeroMemory(&_Status, sizeof(_Status)); };

    BOOL ReportStatus(ServiceStatusHandler handler, DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);

    SERVICE_STATUS GetStatus() { return _Status; };

private:
    SERVICE_STATUS _Status;
};

class ServiceCore final
{
public:
    ServiceCore(EventLogger& logger);
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

    DWORD _dwControlCode;

    ServiceStatusHandler _handler;
    ServiceStatus _status;

    Event _event;
    EventLogger& _logger;
};

