//
// ServiceCommand.cpp
//

#include "pch.h"
#include "ServiceCommand.h"

BOOL ServiceCommand::Command(LPCTSTR lpctszCommand, LPCTSTR lpctszOption)
{
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
        Start();
    }
    else if (_tcscmp(lpctszCommand, COMMAND_STOP) == 0)
    {
        Stop();
    }
    else
    {
        LPCTSTR lpctszMsg = _T("不明なコマンドです。");
        _tprintf(_T("%s:%s\n"), lpctszMsg, lpctszCommand);

        ret = FALSE;
    }

    return ret;
}

BOOL ServiceCommand::Install()
{
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
            _tprintf(_T("%sが失敗しました。\n"), tszMsg);
            break;
        }

        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Create(tszPath);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CreateService()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeDescription(MY_SERVICE_DESCRIPTION);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig2()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("サービスをインストールしました。");
        _tprintf(_T("%s\n"), lpctszMsg);

        ret = TRUE;

    } while (0);

    return ret;
}

BOOL ServiceCommand::Remove()
{
    BOOL ret = FALSE;
    ServiceControlManager scm;
    ServiceControl sc(scm, MY_SERVICE_NAME);

    do
    {
        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(DELETE);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Delete();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("DeleteService()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("サービスをアンインストールしました。");
        _tprintf(_T("%s\n"), lpctszMsg);

        ret = TRUE;

    } while (0);

    return ret;
}

BOOL ServiceCommand::Enable(BOOL bAuto)
{
    BOOL ret = FALSE;
    ServiceControlManager scm;
    ServiceControl sc(scm, MY_SERVICE_NAME);

    do
    {
        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_CHANGE_CONFIG);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig(bAuto == TRUE ? BootSettings::Auto : BootSettings::OnDemand);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = bAuto ? _T("サービスを自動起動にしました。") : _T("サービスを手動起動にしました。");
        _tprintf(_T("%s\n"), lpctszMsg);

        ret = TRUE;

    } while (0);

    return ret;
}

BOOL ServiceCommand::Disable()
{
    BOOL ret = FALSE;
    ServiceControlManager scm;
    ServiceControl sc(scm, MY_SERVICE_NAME);

    do
    {
        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_CHANGE_CONFIG);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig(BootSettings::Disable);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("サービスを無効にしました。");
        _tprintf(_T("%s\n"), lpctszMsg);

        ret = TRUE;

    } while (0);

    return ret;
}

BOOL ServiceCommand::ChangeDescription(LPCTSTR lpctszDescription)
{
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
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_CHANGE_CONFIG);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeDescription(lpctszDescription);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig2()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("サービスの説明を変更しました。");
        _tprintf(_T("%s %s\n"), lpctszMsg, lpctszDescription);

        ret = TRUE;

    } while (0);

    return ret;
}

BOOL ServiceCommand::Start()
{
    BOOL ret = FALSE;
    ServiceControlManager scm;
    ServiceControl sc(scm, MY_SERVICE_NAME);

    do
    {
        ret = scm.Open();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenSCManager()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_ALL_ACCESS);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg;
        QueryResponse response = sc.Query(QueryResponse::StopPending);
        switch (response)
        {
            case QueryResponse::Error:
                lpctszMsg = _T("サービスの起動確認中にエラーが発生しました。");
                break;

            case QueryResponse::Timeout:
                lpctszMsg = _T("サービスの起動確認中にタイムアウトが発生しました。");
                break;

            case QueryResponse::Stopped:
                lpctszMsg = _T("サービスは停止しています。");
                break;

            case QueryResponse::StartPending:
            case QueryResponse::Running:
            case QueryResponse::ContinuePending:
                lpctszMsg = _T("サービスは既に起動しています。");
                break;

            case QueryResponse::PausePending:
            case QueryResponse::Paused:
                lpctszMsg = _T("サービスは中断状態です。");
                break;

            case QueryResponse::StopPending:
            default:
                lpctszMsg = _T("ランタイムエラーが発生しました。");
                break;
        }

        _tprintf(_T("%s\n"), lpctszMsg);

        if (response == QueryResponse::Stopped)
        {
            ret = sc.Start();
            if (!ret)
            {
                LPCTSTR lpctszMsg = _T("StartService()");
                _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
                break;
            }

            response = sc.Query(QueryResponse::StartPending);
            if (response == QueryResponse::Running)
            {
                lpctszMsg = _T("サービスを開始しました。");
            }
            else
            {
                lpctszMsg = _T("サービス開始に失敗しました。");
            }

            _tprintf(_T("%s\n"), lpctszMsg);
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%sが失敗しました。\n"), lpctszMsg);
            break;
        }

        ret = TRUE;

    } while (0);

    return ret;
}

BOOL ServiceCommand::Stop()
{
    // TODO:かなりムズいです。
    return TRUE;
}
