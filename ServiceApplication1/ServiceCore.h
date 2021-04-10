//
// ServiceCore.h
//

#pragma once

#define MY_SERVICE_NAME _T("ServiceApplication1")
#define MY_SERVICE_DISPLAY_NAME _T("サンプルのサービス")
#define MY_SERVICE_DESCRIPTION _T("これはサンプルのサービスプログラムです。")

#define EVENT_STOP _T("SERVICE_STOP_EVENT")

#define COMMAND_INSTALL _T("/install")
#define COMMAND_REMOVE _T("/remove")
#define COMMAND_ENABLE _T("/enable")
#define COMMAND_ONDEMAND _T("/ondemand")
#define COMMAND_DISABLE _T("/disable")
#define COMMAND_CHANGE_DESCRIPTION _T("/description")
#define COMMAND_START _T("/start")

#define MAX_MESSAGE_LEN 256

#include "Main.h"
#include "Event.hpp"
#include "EventLogger.h"
#include "resource.h"

enum class BootSettings
{
    OnDemand = SERVICE_DEMAND_START,
    Auto = SERVICE_AUTO_START,
    Disable = SERVICE_DISABLED
};

enum class QueryResponse
{
    Error = -1,
    Timeout = 0,
    Stopped = SERVICE_STOPPED,
    StartPending = SERVICE_START_PENDING,
    StopPending = SERVICE_STOP_PENDING,
    Running = SERVICE_RUNNING,
    ContinuePending = SERVICE_CONTINUE_PENDING,
    PausePending = SERVICE_PAUSE_PENDING,
    Paused = SERVICE_PAUSED
};

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

    BOOL Create(LPCTSTR lpctszFilePath, BOOL bAutoStart = TRUE)
    {
        _handle = ::CreateService(
            _manager.GetHandle(),
            MY_SERVICE_NAME,
            MY_SERVICE_DISPLAY_NAME,
            SERVICE_ALL_ACCESS,
            SERVICE_WIN32_OWN_PROCESS,
            bAutoStart ? SERVICE_AUTO_START : SERVICE_DEMAND_START,
            SERVICE_ERROR_NORMAL,
            lpctszFilePath,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr);

        return _handle == nullptr ? FALSE : TRUE;
    };

    BOOL Open(DWORD dwDesiredAccess)
    {
        _handle = ::OpenService(
            _manager.GetHandle(),
            _tszServiceName,
            dwDesiredAccess);

        return _handle == nullptr ? FALSE : TRUE;
    };

    QueryResponse Query(QueryResponse wait)
    {
        QueryResponse response = QueryResponse::Error;
        BOOL ret;
        SERVICE_STATUS_PROCESS status{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        DWORD dwBytesNeeded;

        DWORD dwWaitTime = 0;
        DWORD dwStartTickCount = GetTickCount();
        DWORD dwCheckPointBefore = 0;

        do
        {
            dwWaitTime = status.dwWaitHint / 10;
            if (dwWaitTime < 1000) dwWaitTime = 1000;
            else if (dwWaitTime > 10000) dwWaitTime = 10000;

            Sleep(dwWaitTime);

            ret = ::QueryServiceStatusEx(
                _handle,                        // handle to service 
                SC_STATUS_PROCESS_INFO,         // information level
                (LPBYTE)&status,                // address of structure
                sizeof(SERVICE_STATUS_PROCESS), // size of structure
                &dwBytesNeeded);                // size needed if buffer is too small
            
            if (ret == FALSE)
            {
                response = QueryResponse::Error;
                break;
            }
            else
            {
                // SERVICE_STOPPED              停止
                // SERVICE_START_PENDING        開始中
                // SERVICE_STOP_PENDING         停止中
                // SERVICE_RUNNING              開始
                // SERVICE_CONTINUE_PENDING     再開中
                // SERVICE_PAUSE_PENDING        中断中
                // SERVICE_PAUSED               中断
                // 停止中の場合、待機して停止かタイムアウトになるのを待ちます。
                // 開始中の場合、待機して開始かタイムアウトになるのを待ちます。
                response = static_cast<QueryResponse>(status.dwCurrentState);
                if (response != wait) break;
            }

            if (status.dwCheckPoint > dwCheckPointBefore)
            {
                // Continue to wait and check.
                dwStartTickCount = GetTickCount();
                dwCheckPointBefore = status.dwCheckPoint;
            }
            else
            {
                if (GetTickCount() - dwStartTickCount > status.dwWaitHint)
                {
                    response = QueryResponse::Timeout;
                    break;
                }
            }

        } while (response == wait);

        return response;
    }

    BOOL Start()
    {
        return ::StartService(_handle, 0, nullptr);
    }

    BOOL ChangeConfig(BootSettings bootType)
    {
        return ::ChangeServiceConfig(
            _handle,           // handle of service 
            SERVICE_NO_CHANGE, // service type: no change 
            (DWORD)bootType,   // service start type 
            SERVICE_NO_CHANGE, // error control: no change 
            nullptr,           // binary path: no change 
            nullptr,           // load order group: no change 
            nullptr,           // tag ID: no change 
            nullptr,           // dependencies: no change 
            nullptr,           // account name: no change 
            nullptr,           // password: no change 
            nullptr);          // display name: no change
    };

    BOOL ChangeDescription(LPCTSTR lpctszServiceDescription)
    {
        SERVICE_DESCRIPTION service_description = { (LPTSTR)lpctszServiceDescription };

        return ::ChangeServiceConfig2(
            _handle,
            SERVICE_CONFIG_DESCRIPTION,
            &service_description);
    }

    //! @sa https://togarasi.wordpress.com/2008/05/17/%E3%82%B5%E3%83%BC%E3%83%93%E3%82%B9%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%A0%E3%81%AE%E8%87%AA%E5%8B%95%E8%B5%B7%E5%8B%95%E3%82%92%E9%81%85%E5%BB%B6%E5%AE%9F%E8%A1%8C%E3%81%AB%E5%A4%89%E6%9B%B4/
    BOOL ChangeDelayedAutoStart()
    {
        // 自動（遅延開始）について
        // 通常の自動開始サービスの約２分後に開始されるもの。
        // 自動→自動(遅延開始)なので、事前に自動になっていることが前提。
        SERVICE_DELAYED_AUTO_START_INFO service_delayed_auto_start_info = { 0 };

        return ::ChangeServiceConfig2(
            _handle,
            SERVICE_CONFIG_DELAYED_AUTO_START_INFO,
            &service_delayed_auto_start_info);
    }

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

    BOOL Command(LPCTSTR lpctszCommand, LPCTSTR lpctszOption = nullptr);

    BOOL Entry();
    BOOL Install();
    BOOL Remove();
    BOOL Enable(BOOL bAuto);
    BOOL Disable();
    BOOL ChangeDescription(LPCTSTR lpctszDescription);
    BOOL CommandStart();

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

