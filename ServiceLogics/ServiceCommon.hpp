//
// ServiceCommon.hpp
//

#pragma once

#include "ServiceInfo.h"

#define LOG_ROOT _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\ServiceApplication")

#define ENTRY_MAX_SIZE _T("MaxSize")
#define LOG_MAX_SIZE 0x1400000

#define ENTRY_RETENTION _T("Retention")
#define LOG_RETENTION 0

#define ENTRY_FILE _T("File")
#define LOG_FILE _T("%SystemRoot%\\System32\\Winevt\\Logs\\ServiceApplication.evtx")

#define EVENT_ROOT _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\ServiceApplication")

#define ENTRY_CATEGORY_COUNT _T("CategoryCount")
#define EVENT_CATEGORY_COUNT 4

#define ENTRY_CATEGORY_MESSAGE_FILE _T("CategoryMessageFile")
#define EVENT_CATEGORY_MESSAGE_FILE _T("ServiceMessage.dll")

#define ENTRY_EVENT_MESSAGE_FILE _T("EventMessageFile")
#define EVENT_EVENT_MESSAGE_FILE _T("ServiceMessage.dll")

#define ENTRY_TYPES_SUPPORTED _T("TypesSupported")
#define EVENT_TYPES_SUPPORTED 7

#define ERROR_REGISTRY_REGISTER _T("レジストリの登録に失敗しました。")

#define MAX_MESSAGE_LEN 256
#define MAX_STATE_TRANSITION_TIME 3000

VOID WINAPI PrintMessage(
    LPCTSTR lpctszMessage,
    DWORD dwErrorCode = GetLastError(),
    LPCTSTR lpctszOption = nullptr);

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

class DECLSPEC ServiceHandler
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

    virtual BOOL WINAPI Close()
    {
        return ::CloseServiceHandle(_handle);
    }

    virtual SC_HANDLE WINAPI GetHandle() { return _handle; };

protected:
    SC_HANDLE _handle;
};

class DECLSPEC ServiceControlManager final : public ServiceHandler
{
public:
    BOOL WINAPI Open()
    {
        _handle = ::OpenSCManager(
            nullptr,
            nullptr,
            SC_MANAGER_ALL_ACCESS);

        return _handle == nullptr ? FALSE : TRUE;
    };
};

class DECLSPEC ServiceControl final : public ServiceHandler
{
public:
    ServiceControl(ServiceControlManager& manager, ServiceInfo& si)
        : _manager(manager), _si(si)
    {
	};

    BOOL WINAPI Create(LPCTSTR lpctszFilePath, BOOL bAutoStart = TRUE)
    {
        _handle = ::CreateService(
            _manager.GetHandle(),
            _si.GetName(),
            _si.GetDisplayName(),
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

    BOOL WINAPI Open(DWORD dwDesiredAccess)
    {
        _handle = ::OpenService(
            _manager.GetHandle(),
            _si.GetName(),
            dwDesiredAccess);

        return _handle == nullptr ? FALSE : TRUE;
    };

    BOOL WINAPI QueryConfig(LPQUERY_SERVICE_CONFIG& lpQueryServiceConfig)
    {
        BOOL ret;
        DWORD dwBytesNeeded = 0;

        ret = ::QueryServiceConfig(_handle, nullptr, 0, &dwBytesNeeded);
        if (!ret && dwBytesNeeded)
        {
            lpQueryServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, dwBytesNeeded);
            ret = ::QueryServiceConfig(_handle, lpQueryServiceConfig, dwBytesNeeded, &dwBytesNeeded);
        }

        return ret;
    }

    BOOL WINAPI QueryStatusEx(SERVICE_STATUS_PROCESS& status)
    {
        DWORD dwBytesNeeded;

        return ::QueryServiceStatusEx(
            _handle,                        // handle to service 
            SC_STATUS_PROCESS_INFO,         // information level
            (LPBYTE)&status,                // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded);                // size needed if buffer is too small
    }

    QueryResponse WINAPI QueryStatus(QueryResponse wait);

    BOOL WINAPI Start()
    {
        return ::StartService(_handle, 0, nullptr);
    }

    BOOL WINAPI Stop()
    {
        SERVICE_STATUS service_status = { 0, 0, 0, 0, 0, 0, 0 };

        return ::ControlService(_handle, SERVICE_CONTROL_STOP, &service_status);
    }

    BOOL WINAPI ChangeConfig(BootSettings bootType)
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

    BOOL WINAPI ChangeConfig2Description()
    {
		SERVICE_DESCRIPTION service_description = { (LPTSTR)_si.GetDescription() };

        return ::ChangeServiceConfig2(
            _handle,
            SERVICE_CONFIG_DESCRIPTION,
            &service_description);
    }

    //! @sa https://togarasi.wordpress.com/2008/05/17/%E3%82%B5%E3%83%BC%E3%83%93%E3%82%B9%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%A0%E3%81%AE%E8%87%AA%E5%8B%95%E8%B5%B7%E5%8B%95%E3%82%92%E9%81%85%E5%BB%B6%E5%AE%9F%E8%A1%8C%E3%81%AB%E5%A4%89%E6%9B%B4/
    BOOL WINAPI ChangeConfig2DelayedAutoStart(BOOL bDelayed = TRUE)
    {
        // 自動（遅延開始）について
        // 通常の自動開始サービスの約２分後に開始されるもの。
        // 自動→自動(遅延開始)なので、事前に自動になっていることが前提。
        SERVICE_DELAYED_AUTO_START_INFO service_delayed_auto_start_info = { bDelayed };

        return ::ChangeServiceConfig2(
            _handle,
            SERVICE_CONFIG_DELAYED_AUTO_START_INFO,
            &service_delayed_auto_start_info);
    }

    BOOL WINAPI Delete()
    {
        return ::DeleteService(_handle);
    };

    ServiceControlManager& GetManager() { return _manager; }

private:
    ServiceControl();

    ServiceControlManager& _manager;
	ServiceInfo& _si;
};

class DECLSPEC ServiceStatusHandler final
{
public:
    ServiceStatusHandler()
    {
        _hService = nullptr;
    };

    ~ServiceStatusHandler()
    {}

    BOOL WINAPI Init(LPCTSTR lpctszServiceName, LPHANDLER_FUNCTION lpfnCtrlHandler)
    {
        _hService = ::RegisterServiceCtrlHandler(lpctszServiceName, lpfnCtrlHandler);
        return _hService == nullptr ? FALSE : TRUE;
    };

    SERVICE_STATUS_HANDLE WINAPI GetHandle() { return _hService; };

private:
    SERVICE_STATUS_HANDLE _hService;
};

class DECLSPEC ServiceStatus final
{
public:
    ServiceStatus()
    {
        _Status = { 0,0,0,0,0,0,0 };
    };

    BOOL WINAPI ReportStatus(
        ServiceStatusHandler handler,
        DWORD dwCurrentState,
        DWORD dwWin32ExitCode,
        DWORD dwWaitHint);

    SERVICE_STATUS WINAPI GetStatus() { return _Status; };

private:
    SERVICE_STATUS _Status;
};
