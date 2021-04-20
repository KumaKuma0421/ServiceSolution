//
// ServiceCore.cpp
//

#include "pch.h"
#include "ServiceCore.h"
#include "Service.h"

ServiceCore::ServiceCore(EventLogger& logger)
    :_logger(logger)
{
    _dwControlCode = 0;
}

ServiceCore::~ServiceCore()
{}

BOOL ServiceCore::Entry(ServiceMain fnServiceMain, ControlHandler fnHandler)
{
    _logger.TraceStart(CATEGORY_SERVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { (LPTSTR)MY_SERVICE_NAME, fnServiceMain },
        { nullptr, nullptr }
    };

    ret = ::StartServiceCtrlDispatcher(DispatchTable);
    if (!ret)
    {
        LPCTSTR lpctszMsg = _T("StartServiceCtrlDispatcher()");
        _logger.ApiError(
            CATEGORY_SERVICE_CORE,
            GetLastError(),
            __FUNCTIONW__,
            lpctszMsg);
        _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
    }

    _fnHandler = fnHandler;

    _logger.TraceFinish(CATEGORY_SERVICE_CORE, __FUNCTIONW__);
    return ret;
}

VOID ServiceCore::Main(DWORD dwArgc, LPTSTR* lptszArgv)
{
    _logger.TraceStart(CATEGORY_SERVICE_CORE, __FUNCTIONW__);

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
            CATEGORY_SERVICE_CORE,
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
            CATEGORY_SERVICE_CORE,
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
            _logger.ApiError(
                CATEGORY_SERVICE_CORE,
                GetLastError(),
                __FUNCTIONW__,
                lpctszMsg);

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
            _logger.ApiError(
                CATEGORY_SERVICE_CORE,
                GetLastError(),
                __FUNCTIONW__,
                lpctszMsg);
            bLoop = FALSE; // この関数を終了します。
            break;
        }
    }

    ReportStatus(SERVICE_STOP_PENDING, NO_ERROR);

    ret = service.Wait();
    if (!ret)
    {
        LPCTSTR lpctszMsg = _T("WaitForSingleObject()");
        _logger.ApiError(
            CATEGORY_SERVICE_CORE,
            GetLastError(),
            __FUNCTIONW__,
            lpctszMsg);
    }

    ret = Exit();
    if (!ret) return;

    ReportStatus(SERVICE_STOPPED);
    _logger.TraceFinish(CATEGORY_SERVICE_CORE, __FUNCTIONW__);
}

VOID ServiceCore::Handler(DWORD dwControlCode)
{
    _logger.TraceStart(CATEGORY_SERVICE_CORE, __FUNCTIONW__);

    _dwControlCode = dwControlCode;

    auto ActionLog = [&](LPCTSTR lpctszMsg)
    {
        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SERVICE_CORE,
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
            _logger.ApiError(
                CATEGORY_SERVICE_CORE,
                GetLastError(),
                __FUNCTIONW__,
                lpctszMsg);
        }
    }

    _logger.TraceFinish(CATEGORY_SERVICE_CORE, __FUNCTIONW__);
}

// ------------
// PRIVATE ZONE
// ------------

BOOL ServiceCore::Init()
{
    _logger.TraceStart(CATEGORY_SERVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ret = _handler.Init(MY_SERVICE_NAME, _fnHandler);
        if (!ret)
        {
            _logger.ApiError(
                CATEGORY_SERVICE_CORE,
                GetLastError(),
                __FUNCTIONW__,
                _T("RegisterServiceCtrlHandler()"));
            break;
        }

        ret = _event.Create(EVENT_STOP);
        if (!ret)
        {
            _logger.ApiError(
                CATEGORY_SERVICE_CORE,
                GetLastError(),
                __FUNCTIONW__,
                _T("CreateEvent()"));
            break;
        }

        ReportStatus(SERVICE_START_PENDING);

        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SERVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            _T("サービスを初期化しました。"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SERVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Start()
{
    _logger.TraceStart(CATEGORY_SERVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ReportStatus(SERVICE_RUNNING);

        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SERVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            _T("サービスを開始します。"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SERVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Stop()
{
    _logger.TraceStart(CATEGORY_SERVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ReportStatus(SERVICE_STOP_PENDING, NO_ERROR);

        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SERVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            _T("サービスを停止します。"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SERVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Suspend()
{
    _logger.TraceStart(CATEGORY_SERVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ReportStatus(SERVICE_PAUSED, NO_ERROR);

        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SERVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            _T("サービスを中断します。"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SERVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Resume()
{
    _logger.TraceStart(CATEGORY_SERVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ReportStatus(SERVICE_RUNNING, NO_ERROR);

        _logger.Log(
            EVENTLOG_SUCCESS,
            CATEGORY_SERVICE_CORE,
            SVC_SUCCESS_SYSTEM,
            2,
            __FUNCTIONW__,
            _T("サービスを再開します。"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SERVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Exit()
{
    _logger.TraceStart(CATEGORY_SERVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        ret = _event.Close();
        if (!ret)
        {
            _logger.ApiError(
                CATEGORY_SERVICE_CORE,
                GetLastError(),
                __FUNCTIONW__,
                _T("CloseHandle()"));
            break;
        }

        ret = TRUE;

    } while (0);

    if (ret) ReportStatus(SERVICE_STOPPED, NO_ERROR);

    _logger.Log(
        EVENTLOG_SUCCESS,
        CATEGORY_SERVICE_CORE,
        SVC_SUCCESS_SYSTEM,
        2,
        __FUNCTIONW__,
        _T("サービスを終了しました。"));

    _logger.TraceFinish(CATEGORY_SERVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::ReportStatus(
    DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwWaitHint)
{
    TCHAR tszMsg[MAX_MESSAGE_LEN];

    wsprintf(
        tszMsg,
        _T("SetServiceStatus(%d, %d, %d)"),
        dwCurrentState, dwWin32ExitCode, dwWaitHint);

    BOOL ret = _status.ReportStatus(
        _handler,
        dwCurrentState,
        dwWin32ExitCode,
        dwWaitHint);
    if (!ret)
    {
        _logger.Log(
            EVENTLOG_ERROR_TYPE,
            CATEGORY_SERVICE_CORE,
            SVC_ERROR_API,
            2,
            __FUNCTIONW__,
            tszMsg);
    }
    else
    {
        _logger.Trace(CATEGORY_SERVICE_CORE, _T("ReportService"), tszMsg);
    }

    return ret;
}