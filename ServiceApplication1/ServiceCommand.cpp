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
        TCHAR tszPath[MAX_PATH] = { 0 };
        DWORD dwRet = ::GetModuleFileName(nullptr, tszPath, MAX_PATH);
        if (!dwRet)
        {
            TCHAR tszMsg[MAX_MESSAGE_LEN] = { 0 };
            wsprintf(tszMsg, _T("GetModuleFileName(%s)"), tszPath);
            _tprintf(_T("%s�����s���܂����B\n"), tszMsg);
        }
        else
            Install(tszPath);
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
        ChangeConfig2Description(lpctszOption);
    }
    else if (_tcscmp(lpctszCommand, COMMAND_CHANGE_DELAYED_AUTO_START) == 0)
    {
        ChangeConfig2DelayedAutoStart();
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
        LPCTSTR lpctszMsg = _T("�s���ȃR�}���h�ł��B");
        _tprintf(_T("%s:%s\n"), lpctszMsg, lpctszCommand);

        ret = FALSE;
    }

    return ret;
}

BOOL ServiceCommand::Install(LPCTSTR lpctszModulePath)
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
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Create(lpctszModulePath);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CreateService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig2Description(MY_SERVICE_DESCRIPTION);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig2()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("�T�[�r�X���C���X�g�[�����܂����B");
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
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(DELETE);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Delete();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("DeleteService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("�T�[�r�X���A���C���X�g�[�����܂����B");
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
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_CHANGE_CONFIG);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig(bAuto == TRUE ? BootSettings::Auto : BootSettings::OnDemand);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig2DelayedAutoStart(FALSE);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig2()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = bAuto ? _T("�T�[�r�X�������N���ɂ��܂����B") : _T("�T�[�r�X���蓮�N���ɂ��܂����B");
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
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_CHANGE_CONFIG);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig(BootSettings::Disable);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig2DelayedAutoStart(FALSE);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig2()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("�T�[�r�X�𖳌��ɂ��܂����B");
        _tprintf(_T("%s\n"), lpctszMsg);

        ret = TRUE;

    } while (0);

    return ret;
}

BOOL ServiceCommand::ChangeConfig2Description(LPCTSTR lpctszDescription)
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
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_CHANGE_CONFIG);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig2Description(lpctszDescription);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig2()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("�T�[�r�X�̐�����ύX���܂����B");
        _tprintf(_T("%s %s\n"), lpctszMsg, lpctszDescription);

        ret = TRUE;

    } while (0);

    return ret;
}

BOOL ServiceCommand::ChangeConfig2DelayedAutoStart()
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
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_CHANGE_CONFIG);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }
        
        ret = sc.ChangeConfig(BootSettings::Auto);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.ChangeConfig2DelayedAutoStart();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("ChangeServiceConfig2()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("�T�[�r�X�������N��(�x��)�ɂ��܂����B");
        _tprintf(_T("%s\n"), lpctszMsg);

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
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_ALL_ACCESS);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg;
        QueryResponse response = sc.Query(QueryResponse::StopPending);
        switch (response)
        {
            case QueryResponse::Error:
                lpctszMsg = _T("�T�[�r�X�̋N���m�F���ɃG���[���������܂����B");
                break;

            case QueryResponse::Timeout:
                lpctszMsg = _T("�T�[�r�X�̋N���m�F���Ƀ^�C���A�E�g���������܂����B");
                break;

            case QueryResponse::Stopped:
                lpctszMsg = _T("�T�[�r�X�͒�~���Ă��܂��B");
                break;

            case QueryResponse::StartPending:
            case QueryResponse::Running:
            case QueryResponse::ContinuePending:
                lpctszMsg = _T("�T�[�r�X�͊��ɋN�����Ă��܂��B");
                break;

            case QueryResponse::PausePending:
            case QueryResponse::Paused:
                lpctszMsg = _T("�T�[�r�X�͒��f��Ԃł��B");
                break;

            case QueryResponse::StopPending:
            default:
                lpctszMsg = _T("�����^�C���G���[���������܂����B");
                break;
        }

        _tprintf(_T("%s\n"), lpctszMsg);

        if (response == QueryResponse::Stopped)
        {
            ret = sc.Start();
            if (!ret)
            {
                LPCTSTR lpctszMsg = _T("StartService()");
                _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
                break;
            }

            response = sc.Query(QueryResponse::StartPending);
            if (response == QueryResponse::Running)
            {
                lpctszMsg = _T("�T�[�r�X���J�n���܂����B");
            }
            else
            {
                lpctszMsg = _T("�T�[�r�X�J�n�Ɏ��s���܂����B");
            }

            _tprintf(_T("%s\n"), lpctszMsg);
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = TRUE;

    } while (0);

    return ret;
}

BOOL ServiceCommand::Stop()
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
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg;
        QueryResponse response = sc.Query(QueryResponse::StopPending);
        switch (response)
        {
            case QueryResponse::Error:
                lpctszMsg = _T("�T�[�r�X�̋N���m�F���ɃG���[���������܂����B");
                break;

            case QueryResponse::Timeout:
                lpctszMsg = _T("�T�[�r�X�̋N���m�F���Ƀ^�C���A�E�g���������܂����B");
                break;

            case QueryResponse::Stopped:
                lpctszMsg = _T("�T�[�r�X�͊��ɒ�~���Ă��܂��B");
                break;

            case QueryResponse::StartPending:
            case QueryResponse::Running:
            case QueryResponse::ContinuePending:
                lpctszMsg = _T("�T�[�r�X�͋N�����Ă��܂��B");
                break;

            case QueryResponse::PausePending:
            case QueryResponse::Paused:
                lpctszMsg = _T("�T�[�r�X�͒��f��Ԃł��B");
                break;

            case QueryResponse::StopPending:
            default:
                lpctszMsg = _T("�����^�C���G���[���������܂����B");
                break;
        }

        _tprintf(_T("%s\n"), lpctszMsg);

        if (response == QueryResponse::StartPending ||
            response == QueryResponse::Running ||
            response == QueryResponse::ContinuePending)
        {
            ret = sc.Stop();
            if (!ret)
            {
                LPCTSTR lpctszMsg = _T("StartService()");
                _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
                break;
            }

            response = sc.Query(QueryResponse::StopPending);
            if (response == QueryResponse::Stopped)
            {
                lpctszMsg = _T("�T�[�r�X���~���܂����B");
            }
            else
            {
                lpctszMsg = _T("�T�[�r�X��~�Ɏ��s���܂����B");
            }

            _tprintf(_T("%s\n"), lpctszMsg);
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = TRUE;

    } while (0);

    return ret;
}

// ------------
// PRIVATE ZONE
// ------------

BOOL TemplateAction(DWORD dwOpenParam, TemplateMethod method)
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
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Open(dwOpenParam);
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("OpenService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Delete();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("DeleteService()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            LPCTSTR lpctszMsg = _T("CloseServiceHandle()");
            _tprintf(_T("%s�����s���܂����B\n"), lpctszMsg);
            break;
        }

        LPCTSTR lpctszMsg = _T("�T�[�r�X���A���C���X�g�[�����܂����B");
        _tprintf(_T("%s\n"), lpctszMsg);

        ret = TRUE;

    } while (0);

    return ret;
}