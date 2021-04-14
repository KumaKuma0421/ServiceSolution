//
// ServiceCommon.hpp
//

#pragma once

#define MY_SERVICE_NAME _T("ServiceApplication1")
#define MY_SERVICE_DISPLAY_NAME _T("サンプルのサービス")
#define MY_SERVICE_DESCRIPTION _T("これはサンプルのサービスプログラムです。")

#define MAX_MESSAGE_LEN 256
#define MAX_STATE_TRANSITION_TIME 3000

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
            MY_SERVICE_NAME,
            dwDesiredAccess);

        return _handle == nullptr ? FALSE : TRUE;
    };

    QueryResponse Query(QueryResponse wait);

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
    // TODO:未使用！
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
    }

    BOOL Init(LPCTSTR lpctszServiceName, LPHANDLER_FUNCTION lpfnCtrlHandler)
    {
        _hService = ::RegisterServiceCtrlHandler(lpctszServiceName, lpfnCtrlHandler);
        return _hService == nullptr ? FALSE : TRUE;
    };

    SERVICE_STATUS_HANDLE GetHandle() { return _hService; };

private:
    SERVICE_STATUS_HANDLE _hService;
};

class ServiceStatus final
{
public:
    ServiceStatus()
    {
        _Status = { 0,0,0,0,0,0,0 };
    };

    BOOL ReportStatus(
        ServiceStatusHandler handler,
        DWORD dwCurrentState,
        DWORD dwWin32ExitCode,
        DWORD dwWaitHint);

    SERVICE_STATUS GetStatus() { return _Status; };

private:
    SERVICE_STATUS _Status;
};
