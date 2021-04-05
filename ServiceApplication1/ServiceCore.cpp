//
// ServiceCore.cpp
//

#include "pch.h"
#include "ServiceCore.h"
#include "Service.h"

BOOL ServiceStatus::ReportStatus(ServiceStatusHandler handler, DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    BOOL ret = FALSE;

    _Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    _Status.dwServiceSpecificExitCode = 0;
    _Status.dwCurrentState = dwCurrentState;
    _Status.dwWin32ExitCode = dwWin32ExitCode;
    _Status.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        _Status.dwControlsAccepted = 0;
    else
        _Status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
        _Status.dwCheckPoint = 0;
    else
        _Status.dwCheckPoint++;

    return ::SetServiceStatus(handler.GetHandle(), &_Status);
};

ServiceCore::ServiceCore(EventLogger& logger)
    :_logger(logger)
{
    _dwControlCode = 0;
}

ServiceCore::~ServiceCore()
{
}

BOOL ServiceCore::Command(LPCTSTR lpctszCommand)
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
    else
    {
        _logger.Log(EVENTLOG_ERROR_TYPE, CATEGORY_SEVICE_CORE, SVC_ERROR_RUNTIME, 3, __FUNCTIONW__, _T("�s���ȃR�}���h�ł��B"), lpctszCommand);
        ret = FALSE;
    }

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Entry()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    TCHAR tszMsg[MAX_MESSAGE_LEN] = { 0 };

    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { (LPTSTR)SERVICE_NAME, SvcMain },
        { nullptr, nullptr }
    };

    ret = ::StartServiceCtrlDispatcher(DispatchTable);
    if (!ret)
    {
        _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, _T("StartServiceCtrlDispatcher()"));
    }

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Install()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    TCHAR tszMsg[MAX_MESSAGE_LEN] = { 0 };
    TCHAR tszPath[MAX_PATH] = { 0 };
    ServiceControlManager scm;
    ServiceControl sc(scm, SERVICE_NAME);

    do
    {
        DWORD dwRet = ::GetModuleFileName(nullptr, tszPath, MAX_PATH);
        if (!dwRet)
        {
            // �G���[���O����ʏo�͂�
            _logger.ApiError(CATEGORY_SEVICE_CORE, GetLastError(), __FUNCTIONW__, _T("GetModuleFileName()"));
            break;
        }

        ret = scm.Open();
        if (!ret)
        {
            // �G���[���O����ʏo�͂�
            break;
        }

        ret = sc.Create(tszPath);
        if (!ret)
        {
            // �G���[���O����ʏo�͂�
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            // �T�[�r�X�N���[�Y���s����ʏo��
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            // �T�[�r�X�R���g���[���}�l�[�W���̃N���[�Y���s����ʏo��
            break;
        }

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Remove()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    TCHAR tszMsg[MAX_MESSAGE_LEN] = { 0 };
    ServiceControlManager scm;
    ServiceControl sc(scm, SERVICE_NAME);

    do
    {
        ret = scm.Open();
        if (!ret)
        {
            // �G���[������ʏo��
            break;
        }

        ret = sc.Open();
        if (!ret)
        {
            // �G���[������ʏo��
            break;
        }

        ret = sc.Delete();
        if (!ret)
        {
            // �G���[������ʏo��
            break;
        }

        ret = sc.Close();
        if (!ret)
        {
            // �T�[�r�X�̃N���[�Y���s����ʏo��
            break;
        }

        ret = scm.Close();
        if (!ret)
        {
            // �T�[�r�X�}�l�[�W���̃N���[�Y���s����ʏo��
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

    ret = Start();
    if (!ret) return;

    Service service(_logger);
    ret = service.Start(STOP_EVENT);
    if (!ret) return;

    _logger.Log(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 2, __FUNCTIONW__, _T("�T�[�r�X�����s���ł��B"));

    BOOL bLoop = TRUE;
    while (bLoop)
    {
        DWORD dwRet = _event.Wait();
        if (dwRet == WAIT_FAILED)
        {
            ReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
            bLoop = FALSE;
            break;
        }

        switch (_dwControlCode)
        {
            case SERVICE_CONTROL_STOP:
            case SERVICE_CONTROL_SHUTDOWN:
                Stop();
                service.Stop();
                bLoop = FALSE; // ���̊֐����I�����܂��B
                break;

            case SERVICE_CONTROL_PAUSE:
            case SERVICE_CONTROL_PRESHUTDOWN:
                service.Suspend();
                Suspend();
                break;

            case SERVICE_CONTROL_CONTINUE:
                service.Resume();
                Resume();
                break;

            case WAIT_FAILED:
                break;

            default:
                ReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
                break;
        }
        
        ret = _event.Reset();
        if (!ret) return;
    }

    ret = service.Wait();
    if (!ret) return;

    ret = Exit();
    if (!ret) return;

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
            _T("�T�[�r�X����R�[���o�b�N����M���܂����B"),
            lpctszMsg);
    };

    BOOL ret = TRUE;
    switch (_dwControlCode)
    {
        case SERVICE_CONTROL_STOP:
            ActionLog(_T("STOP"));
            break;

        case SERVICE_CONTROL_SHUTDOWN:
            ActionLog(_T("SHUTDOWN"));
            break;

        case SERVICE_CONTROL_PAUSE:
            ActionLog(_T("PAUSE"));
            break;

        case SERVICE_CONTROL_PRESHUTDOWN:
            ActionLog(_T("PRESHUTDOWN"));
            break;

        case SERVICE_CONTROL_CONTINUE:
            ActionLog(_T("CONTINUE"));
            break;

        default:
            ActionLog(_T("INVALID CALLBACK"));
            ret = FALSE;
            break;
    }

    if (ret)
    {
        ret = _event.Set();
        if (!ret) return;
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
    TCHAR tszMsg[MAX_MESSAGE_LEN] = { 0 };

    do
    {
        ret = _handler.Init(SERVICE_NAME, CtrlHandler);
        if (!ret)
        {
            // �C�x���g�r���[�A�[�ɃG���[����������
            break;
        }

        ret = _event.Create(EVENT_STOP);
        if (!ret)
        {
            // �C�x���g�r���[�A�[�ɃG���[����������
            break;
        }

        ReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

        _logger.Log(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 2, __FUNCTIONW__, _T("�T�[�r�X�����������܂����B"));
        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Exit()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    TCHAR tszMsg[MAX_MESSAGE_LEN] = { 0 };

    do
    {
        ret = _event.Close();
        if (!ret)
        {
            // �C�x���g�r���[�A�[�ɃG���[����������
            break;
        }

        ret = _handler.Exit();
        if (!ret)
        {
            // �C�x���g�r���[�A�[�ɃG���[����������
            break;
        }

        ret = TRUE;

    } while (0);

    if (ret) ReportStatus(SERVICE_STOPPED, NO_ERROR, 3000);

    _logger.Log(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 2, __FUNCTIONW__, _T("�T�[�r�X���I�����܂����B"));

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Start()
{
    _logger.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        // �T�[�r�X�J�n�����Ŏ��s�����ꍇ�A�X�e�[�^�X��
        // SERVICE_START_PENDING��SERVICE_STOPPED�ɕύX����B

        ReportStatus(SERVICE_RUNNING, NO_ERROR, 3000);

        _logger.Log(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 2, __FUNCTIONW__, _T("�T�[�r�X���J�n���܂��B"));
        
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
        ReportStatus(SERVICE_STOPPED, NO_ERROR, 0);

        _logger.Log(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 2, __FUNCTIONW__, _T("�T�[�r�X���~���܂��B"));

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
        ReportStatus(SERVICE_PAUSED, NO_ERROR, 0);

        _logger.Log(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 2, __FUNCTIONW__, _T("�T�[�r�X�𒆒f���܂��B"));

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
        ReportStatus(SERVICE_RUNNING, NO_ERROR, 0);

        _logger.Log(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 2, __FUNCTIONW__, _T("�T�[�r�X���ĊJ���܂��B"));

        ret = TRUE;

    } while (0);

    _logger.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::ReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    BOOL ret = _status.ReportStatus(_handler, dwCurrentState, dwWin32ExitCode, dwWaitHint);
    if (!ret)
    {
        _logger.Log(EVENTLOG_ERROR_TYPE, CATEGORY_SEVICE_CORE, SVC_ERROR_API, 2, __FUNCTIONW__, _T("�T�[�r�X�̏�ԑJ�ڂɎ��s���܂����B"));
    }

    return ret;
}