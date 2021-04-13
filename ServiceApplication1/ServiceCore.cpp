//
// ServiceCore.cpp
//

#include "pch.h"
#include "ServiceCore.h"
#include "Service.h"

BOOL ServiceStatus::ReportStatus(
    ServiceStatusHandler handler,
    DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwWaitHint)
{
    BOOL ret = FALSE;

    _Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    _Status.dwServiceSpecificExitCode = 0;
    _Status.dwCurrentState = dwCurrentState;
    _Status.dwWin32ExitCode = dwWin32ExitCode;

    if (dwCurrentState == SERVICE_START_PENDING)
        _Status.dwControlsAccepted = 0;
    else
        _Status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

    if ((dwCurrentState == SERVICE_RUNNING) ||
        (dwCurrentState == SERVICE_STOPPED) ||
        (dwCurrentState == SERVICE_PAUSED))
    {
        _Status.dwCheckPoint = 0;
        _Status.dwWaitHint = 0;
    }
    else
    {
        _Status.dwCheckPoint++;
        _Status.dwWaitHint = dwWaitHint;
    }

    return ::SetServiceStatus(handler.GetHandle(), &_Status);
};

ServiceCore::ServiceCore(EventLogger& logger)
    :_logger(logger)
{
    _dwControlCode = 0;
}

ServiceCore::~ServiceCore()
{}

BOOL ServiceCore::Command(LPCTSTR lpctszCommand, LPCTSTR lpctszOption)
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = TRUE;

    if (_tcscmp(lpctszCommand, COMMAND_INSTALL) == 0)
    {
        Install();
    }
    else if (_tcscmp(lpctszCommand, COMMAND_REMOVE) == 0)
    {
        Remove();
    }
    else if (_tcscmp(lpctszCommand, COMMAND_ENABLE) == 0)
    {
        Enable(TRUE);
    }
    else if (_tcscmp(lpctszCommand, COMMAND_ONDEMAND) == 0)
    {
        Enable(FALSE);
    }
    else if (_tcscmp(lpctszCommand, COMMAND_DISABLE) == 0)
    {
        Disable();
    }
    else if (_tcscmp(lpctszCommand, COMMAND_CHANGE_DESCRIPTION) == 0)
    {
        ChangeDescription(lpctszOption);
    }
    else if (_tcscmp(lpctszCommand, COMMAND_START) == 0)
    {
        CommandStart();
    }
    else
    {
        LPCTSTR lpctszMsg = _T("不明なコマンドです。");
        _logger.Log(
            EVENTLOG_ERROR_TYPE,
            CATEGORY_SEVICE_CORE,
            SVC_ERROR_RUNTIME,
            3,
            __FUNCTIONW__,
            lpctszMsg,
            lpctszCommand);
        _tprintf(_T("%s:%s\n"), lpctszMsg, lpctszCommand);

        ret = FALSE;
    }

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Entry()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { (LPTSTR)MY_SERVICE_NAME, SvcMain },
        { nullptr, nullptr }
    };

    ret = ::StartServiceCtrlDispatcher(DispatchTable);
    if (!ret)
    {
        LPCTSTR lpctszMsg = _T("StartServiceCtrlDispatcher()");
        _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
        _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
    }

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Install()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    TCHAR tszPath[MAX_PATH] = { 0 };
    TCHAR tszMsg[MAX_MESSAGE_LEN] = { 0 };
    ServiceControlManager scm;
    ServiceControl sc(scm, MY_SERVICE_NAME);

    do
    {
        DWORD dwRet = ::GetModuleFileName(nullptr, tszPath, MAX_PATH);
        if (!dwRet)
        {
            wsprintf(tszMsg, _T("GetModuleFileName(%s)"), tszPath);
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, tszMsg);
            _tprintf(_T("%sが失敗しました。\n"), tszMsg);
            break;
        }

        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Create(tszPath);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CreateService()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeDescription(MY_SERVICE_DESCRIPTION);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig2()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("サービスをインストールしました。");
        _logger.Log(
            EVENTLOG_INFORMATION_TYPE,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            3,
            __FUNCTIONW__,
            lpctszMsg,
            MY_SERVICE_NAME);
        _tprintf(_T("%s\n"), lpctszMsg);

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Remove()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    ServiceControlManager scm;
    ServiceControl sc(scm, MY_SERVICE_NAME);

    do
    {
        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(DELETE);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Delete();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("DeleteService()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("サービスをアンインストールしました。");
        _logger.Log(
            EVENTLOG_INFORMATION_TYPE,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            3,
            __FUNCTIONW__,
            lpctszMsg,
            MY_SERVICE_NAME);
        _tprintf(_T("%s\n"), lpctszMsg);

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Enable(BOOL bAuto)
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    ServiceControlManager scm;
    ServiceControl sc(scm, MY_SERVICE_NAME);

    do
    {
        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_CHANGE_CONFIG);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig(bAuto == TRUE ? BootSettings::Auto : BootSettings::OnDemand);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = bAuto ? _T("サービスを自動起動にしました。") : _T("サービスを手動起動にしました。");
        _logger.Log(
            EVENTLOG_INFORMATION_TYPE,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            lpctszMsg);
        _tprintf(_T("%s\n"), lpctszMsg);

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Disable()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    ServiceControlManager scm;
    ServiceControl sc(scm, MY_SERVICE_NAME);

    do
    {
        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_CHANGE_CONFIG);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig(BootSettings::Disable);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("サービスを無効にしました。");
        _logger.Log(
            EVENTLOG_INFORMATION_TYPE,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            lpctszMsg);
        _tprintf(_T("%s\n"), lpctszMsg);

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::ChangeDescription(LPCTSTR lpctszDescription)
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    TCHAR tszMsg[MAX_MESSAGE_LEN] = { 0 };
    ServiceControlManager scm;
    ServiceControl sc(scm, MY_SERVICE_NAME);

    do
    {
        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_CHANGE_CONFIG);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeDescription(lpctszDescription);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig2()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("サービスの説明を変更しました。");
        _logger.Log(
            EVENTLOG_INFORMATION_TYPE,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            3,
            __FUNCTIONW__,
            lpctszMsg,
            lpctszDescription);
        _tprintf(_T("%s %s\n"), lpctszMsg, lpctszDescription);

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::CommandStart()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    ServiceControlManager scm;
    ServiceControl sc(scm, MY_SERVICE_NAME);

    do
    {
        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_ALL_ACCESS);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg;
        WORD wEventType;
        DWORD dwEventID;
        QueryResponse response = sc.Query(QueryResponse::StopPending);
        switch (response)
        {
            case QueryResponse::Error:
                wEventType = EVENTLOG_ERROR_TYPE;
                dwEventID = SVC_ERROR_API;
                lpctszMsg = _T("サービスの起動確認中にエラーが発生しました。");
                break;

            case QueryResponse::Timeout:
                wEventType = EVENTLOG_ERROR_TYPE;
                dwEventID = SVC_ERROR_RUNTIME;
                lpctszMsg = _T("サービスの起動確認中にタイムアウトが発生しました。");
                break;

            case QueryResponse::Stopped:
                wEventType = EVENTLOG_INFORMATION_TYPE;
                dwEventID = SVC_SUCCESS_RUNTIME;
                lpctszMsg = _T("サービスは停止しています。");
                break;

            case QueryResponse::StartPending:
            case QueryResponse::Running:
            case QueryResponse::ContinuePending:
                wEventType = EVENTLOG_INFORMATION_TYPE;
                dwEventID = SVC_SUCCESS_RUNTIME;
                lpctszMsg = _T("サービスは既に起動しています。");
                break;

            case QueryResponse::PausePending:
            case QueryResponse::Paused:
                wEventType = EVENTLOG_WARNING_TYPE;
                dwEventID = SVC_WARNING_RUNTIME;
                lpctszMsg = _T("サービスは中断状態です。");
                break;

            case QueryResponse::StopPending:
            default:
                wEventType = EVENTLOG_ERROR_TYPE;
                dwEventID = SVC_ERROR_RUNTIME;
                lpctszMsg = _T("ランタイムエラーが発生しました。");
                break;
        }

        _logger.Log(
            wEventType,
            CATEGORY_SEVICE_CORE,
            dwEventID,
            2,
            __FUNCTIONW__,
            lpctszMsg);
        _tprintf(_T("%s\n"), lpctszMsg);

        if (response == QueryResponse::Stopped)
        {
            ret = sc.Start();
            if (!ret)
            {
                LPCTSTR lpctszMsg = _T("StartService()");
                _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
                _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
                break;
            }

            response = sc.Query(QueryResponse::StartPending);
            if (response == QueryResponse::Running)
            {
                wEventType = EVENTLOG_INFORMATION_TYPE;
                dwEventID = SVC_SUCCESS_SYSTEM;
                lpctszMsg = _T("サービスを開始しました。");
            }
            else
            {
                wEventType = EVENTLOG_ERROR_TYPE;
                dwEventID = SVC_ERROR_SYSTEM;
                lpctszMsg = _T("サービス開始に失敗しました。");
            }

            _logger.Log(
                wEventType,
                CATEGORY_SEVICE_CORE,
                dwEventID,
                2,
                __FUNCTIONW__,
                lpctszMsg);
            _tprintf(_T("%s\n"), lpctszMsg);
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

VOID ServiceCore::Main(DWORD dwArgc, LPTSTR* lptszArgv)
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret;

    ret = Init();
    if (!ret) return;

    Service service(_logger);
    ret = service.Start(STOP_EVENT);
    if (!ret)
    {
        LPCTSTR lpctszMsg = _T("サービスの実行に失敗しました。終了します。");
        _logger.Log(
            EVENTLOG_ERROR_TYPE,
            CATEGORY_SEVICE_CORE,
            SVC_ERROR_RUNTIME,
            2,
            __FUNCTIONW__,
            lpctszMsg);
        return;
    }
    else
    {
        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_RUNTIME,
            2,
            __FUNCTIONW__, _T("サービスを実行中です。"));
    }

    ret = Start();
    if (!ret) return;

    BOOL bLoop = TRUE;
    while (bLoop)
    {
        DWORD dwRet = _event.Wait();
        if (dwRet == WAIT_FAILED)
        {
            LPCTSTR lpctszMsg = _T("WaitForSingleObject()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);

            ReportStatus(SERVICE_STOPPED);
            bLoop = FALSE; // この関数を終了します。
            break;
        }

        switch (_dwControlCode)
        {
            case SERVICE_CONTROL_STOP:
            case SERVICE_CONTROL_SHUTDOWN:
                service.Stop();
                Stop();
                bLoop = FALSE; // この関数を終了します。
                break;

            case SERVICE_CONTROL_PAUSE:
            case SERVICE_CONTROL_PRESHUTDOWN:
                ReportStatus(SERVICE_PAUSE_PENDING);
                service.Suspend();
                Suspend();
                break;

            case SERVICE_CONTROL_CONTINUE:
                ReportStatus(SERVICE_CONTINUE_PENDING);
                service.Resume();
                Resume();
                break;

            default:
                ReportStatus(SERVICE_STOP_PENDING);
                bLoop = FALSE; // この関数を終了します。
                break;
        }

        ret = _event.Reset();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ResetEvent()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
            bLoop = FALSE; // この関数を終了します。
            break;
        }
    }

    ReportStatus(SERVICE_STOP_PENDING, NO_ERROR);

    ret = service.Wait();
    if (!ret)
    {
        LPCTSTR lpctszMsg = _T("WaitForSingleObject()");
        _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
    }

    //ReportStatus(SERVICE_STOP_PENDING, NO_ERROR);
    //Exit();
    ReportStatus(SERVICE_STOPPED);
    Exit();
    // TODO:上のようにSERVICE_STOPPEDにステータス遷移してからExit()しないと、DeregisterEventSource()
    // がエラー（無効なハンドル）を起こす。
    // 逆にするとDeregisterEventSource()はエラーを返すが、各関数は尻切れトンボにならずにFinishログを
    // 出力する。ここはDeregisterEventSource()を調査してみることに。

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
}

VOID ServiceCore::Handler(DWORD dwControlCode)
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    _dwControlCode = dwControlCode;

    auto ActionLog = [&](LPCTSTR lpctszMsg)
    {
        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            3,
            _T("ServiceCore::Handler"),
            _T("サービス制御コールバックを受信しました。"),
            lpctszMsg);
    };

    BOOL ret = TRUE;
    switch (_dwControlCode)
    {
        case SERVICE_CONTROL_STOP:
            ActionLog(_T("STOP"));
            ReportStatus(SERVICE_STOP_PENDING);
            break;

        case SERVICE_CONTROL_SHUTDOWN:
            ActionLog(_T("SHUTDOWN"));
            ReportStatus(SERVICE_STOP_PENDING);
            break;

        case SERVICE_CONTROL_PAUSE:
            ActionLog(_T("PAUSE"));
            ReportStatus(SERVICE_PAUSE_PENDING);
            break;

        case SERVICE_CONTROL_PRESHUTDOWN:
            ActionLog(_T("PRESHUTDOWN"));
            ReportStatus(SERVICE_STOP_PENDING);
            break;

        case SERVICE_CONTROL_CONTINUE:
            ActionLog(_T("CONTINUE"));
            ReportStatus(SERVICE_CONTINUE_PENDING);
            break;

        default:
            ActionLog(_T("INVALID CALLBACK"));
            ret = FALSE;
            break;
    }

    if (ret)
    {
        ret = _event.Set();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("SetEvent()");
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, lpctszMsg);
        }
    }

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
}

// ------------
// PRIVATE ZONE
// ------------

BOOL ServiceCore::Init()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ret = _handler.Init(MY_SERVICE_NAME, CtrlHandler);
        if (!ret)
        {
            _logger.ApiError(
                CATEGORY_SEVICE_CORE,
                GetLastError(),
                __FUNCTIONW__,
                _T("RegisterServiceCtrlHandler()"));
            break;
        }

        ret = _event.Create(EVENT_STOP);
        if (!ret)
        {
            _logger.ApiError(
                CATEGORY_SEVICE_CORE,
                GetLastError(),
                __FUNCTIONW__,
                _T("CreateEvent()"));
            break;
        }

        ReportStatus(SERVICE_START_PENDING);

        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            _T("サービスを初期化しました。"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Start()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ReportStatus(SERVICE_RUNNING);

        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            _T("サービスを開始します。"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Stop()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ReportStatus(SERVICE_STOP_PENDING, NO_ERROR);

        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            _T("サービスを停止します。"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Suspend()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ReportStatus(SERVICE_PAUSED, NO_ERROR);

        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            _T("サービスを中断します。"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Resume()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ReportStatus(SERVICE_RUNNING, NO_ERROR);

        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SEVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            _T("サービスを再開します。"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Exit()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ret = _event.Close();
        if (!ret)
        {
            _logger.ApiError(
                CATEGORY_SEVICE_CORE,
                GetLastError(),
                __FUNCTIONW__,
                _T("CloseHandle()"));
            break;
        }

        ret = _handler.Exit();
        if (!ret)
        {
            _logger.ApiError(
                CATEGORY_SEVICE_CORE,
                GetLastError(),
                __FUNCTIONW__,
                _T("DeregisterEventSource()"));
            break;
        }

        ret = TRUE;

    } while (0);

    if (ret) ReportStatus(SERVICE_STOPPED, NO_ERROR);

    _logger.Log(
        EVENTLOG_SUCCESS,
        CATEGORY_SEVICE_CORE,
        SVC_SUCCESS_SYSTEM,
        2,
        __FUNCTIONW__,
        _T("サービスを終了しました。"));

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::ReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    BOOL ret = _status.ReportStatus(_handler, dwCurrentState, dwWin32ExitCode, dwWaitHint);
    if (!ret)
    {
        _logger.Log(
            EVENTLOG_ERROR_TYPE,
            CATEGORY_SEVICE_CORE,
            SVC_ERROR_API,
            2,
            __FUNCTIONW__,
            _T("SetServiceStatus()"));
    }

    return ret;
}