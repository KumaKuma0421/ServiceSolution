//
// ServiceCore.cpp
//

#include "pch.h"
#include "ServiceCore.h"
#include "Service.h"

ServiceCore::ServiceCore()
{
    _hServiceStatus = nullptr;
    ::ZeroMemory(&_status, sizeof(_status));
    _dwControlCode = 0;
    _log.Init(SERVICE_NAME);
}

ServiceCore::~ServiceCore()
{
    _log.Exit();
}

BOOL ServiceCore::Command(LPCTSTR lpctszCommand)
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

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
        LPCTSTR lpctszArgs[3] = {
            __FUNCTIONW__,
            lpctszCommand,
            _T("�s���ȃR�}���h�ł��B") // @ToDo �����p�̃��b�Z�[�W���K�v
        };

        _log.Write(EVENTLOG_ERROR_TYPE, CATEGORY_SEVICE_CORE, SVC_ERROR_RUNTIME, 3, (LPCTSTR*)&lpctszArgs);
        ret = FALSE;
    }

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Entry()
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

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
        LPCTSTR lpctszArgs[3] = {
            __FUNCTIONW__,
            _T("StartServiceCtrlDispatcher()�Ɏ��s���܂����B"), // @ToDo �����p�̃��b�Z�[�W���K�v
            _T("")
        };

        _log.Write(EVENTLOG_ERROR_TYPE, CATEGORY_SEVICE_CORE, SVC_ERROR_RUNTIME, 3, (LPCTSTR*)&lpctszArgs);
    }

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Install()
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

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

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Remove()
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

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

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

VOID ServiceCore::Main(DWORD dwArgc, LPTSTR* lptszArgv)
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret;

    ret = Init();
    if (!ret) return;

    ret = Start();
    if (!ret) return;

    Service service;
    ret = service.Start(STOP_EVENT);
    if (!ret) return;

    LPCTSTR lpctszArgs[3] = {
        __FUNCTIONW__,
        _T("�T�[�r�X�����s���ł��B"),
        _T("") // @ToDo �����p�̃��b�Z�[�W���K�v
    };

    _log.Write(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 3, (LPCTSTR*)&lpctszArgs);

    BOOL bLoop = TRUE;
    while (bLoop)
    {
        DWORD dwRet = event.Wait();
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
        
        ret = event.Reset();
        if (!ret) return;
    }

    ret = Exit();
    if (!ret) return;

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
}

VOID ServiceCore::Handler(DWORD dwControlCode)
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    _dwControlCode = dwControlCode;

    auto ActionLog = [&](LPCTSTR lpctszMsg)
    {
        LPCTSTR lpctszArgs[3] = {
           _T("ServiceCore::Handler"),
           _T("�T�[�r�X����R�[���o�b�N����M���܂����B"),
           lpctszMsg // @ToDo �����p�̃��b�Z�[�W���K�v
        };

        _log.Write(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 3, (LPCTSTR*)&lpctszArgs);
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
        ret = event.Set();
        if (!ret) return;
    }

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
}

// ------------
// PRIVATE ZONE
// ------------

BOOL ServiceCore::Init()
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    TCHAR tszMsg[MAX_MESSAGE_LEN] = { 0 };

    do
    {
        _hServiceStatus = ::RegisterServiceCtrlHandler(SERVICE_NAME, CtrlHandler);
        if (!_hServiceStatus)
        {
            // �C�x���g�r���[�A�[�ɃG���[����������
            break;
        }

        ret = event.Create(EVENT_STOP);
        if (!ret)
        {
            // �C�x���g�r���[�A�[�ɃG���[����������
            break;
        }

        ReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

        LPCTSTR lpctszArgs[3] = {
           __FUNCTIONW__,
           _T("�T�[�r�X�����������܂��B"),
           _T("") // @ToDo �����p�̃��b�Z�[�W���K�v
        };

        _log.Write(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 3, (LPCTSTR*)&lpctszArgs);
        ret = TRUE;

    } while (0);

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Exit()
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;
    TCHAR tszMsg[MAX_MESSAGE_LEN] = { 0 };

    do
    {
        ret = event.Close();
        if (!ret)
        {
            // �C�x���g�r���[�A�[�ɃG���[����������
            break;
        }

        ret = ::DeregisterEventSource(_hServiceStatus);
        if (!ret)
        {
            // �C�x���g�r���[�A�[�ɃG���[����������
            break;
        }

        ret = TRUE;

    } while (0);

    if (ret) ReportStatus(SERVICE_STOPPED, NO_ERROR, 3000);

    LPCTSTR lpctszArgs[3] = {
       __FUNCTIONW__,
       _T("�T�[�r�X���I�����܂��B"),
       _T("") // @ToDo �����p�̃��b�Z�[�W���K�v
    };

    _log.Write(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 3, (LPCTSTR*)&lpctszArgs);

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Start()
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        // �T�[�r�X�J�n�����Ŏ��s�����ꍇ�A�X�e�[�^�X��
        // SERVICE_START_PENDING��SERVICE_STOPPED�ɕύX����B

        ReportStatus(SERVICE_RUNNING, NO_ERROR, 3000);

        LPCTSTR lpctszArgs[3] = {
           __FUNCTIONW__,
           _T("�T�[�r�X���J�n���܂��B"),
           _T("") // @ToDo �����p�̃��b�Z�[�W���K�v
        };

        _log.Write(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 3, (LPCTSTR*)&lpctszArgs);
        
        ret = TRUE;

    } while (0);

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Stop()
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        // ��~�����������ɏ���

        ret = TRUE;

    } while (0);

    if (ret)
    {
        ReportStatus(SERVICE_STOPPED, NO_ERROR, 0);

        LPCTSTR lpctszArgs[3] = {
           __FUNCTIONW__,
           _T("�T�[�r�X���~���܂��B"),
           _T("") // @ToDo �����p�̃��b�Z�[�W���K�v
        };

        _log.Write(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 3, (LPCTSTR*)&lpctszArgs);
    }

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Suspend()
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        // ���f�����������ɏ���

        ret = TRUE;

    } while (0);

    if (ret)
    {
        ReportStatus(SERVICE_PAUSED, NO_ERROR, 0);

        LPCTSTR lpctszArgs[3] = {
           __FUNCTIONW__,
           _T("�T�[�r�X�𒆒f���܂��B"),
           _T("") // @ToDo �����p�̃��b�Z�[�W���K�v
        };

        _log.Write(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 3, (LPCTSTR*)&lpctszArgs);
    }

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::Resume()
{
    _log.TraceStart(CATEGORY_SEVICE_CORE, __FUNCTIONW__);

    BOOL ret = FALSE;

    do
    {
        // �ĊJ����������

        ret = TRUE;

    } while (0);

    if (ret)
    {
        ReportStatus(SERVICE_RUNNING, NO_ERROR, 0);
        
        LPCTSTR lpctszArgs[3] = {
           __FUNCTIONW__,
           _T("�T�[�r�X���ĊJ���܂��B"),
           _T("") // @ToDo �����p�̃��b�Z�[�W���K�v
        };

        _log.Write(EVENTLOG_SUCCESS, CATEGORY_SEVICE_CORE, SVC_SUCCESS_SYSTEM, 3, (LPCTSTR*)&lpctszArgs);
    }

    _log.TraceFinish(CATEGORY_SEVICE_CORE, __FUNCTIONW__);
    return ret;
}

BOOL ServiceCore::ReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    BOOL ret = FALSE;

    _status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    _status.dwServiceSpecificExitCode = 0;
    _status.dwCurrentState = dwCurrentState;
    _status.dwWin32ExitCode = dwWin32ExitCode;
    _status.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        _status.dwControlsAccepted = 0;
    else
        _status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
        _status.dwCheckPoint = 0;
    else
        _status.dwCheckPoint++;

    ret = ::SetServiceStatus(_hServiceStatus, &_status);
    if (!ret)
    {
        LPCTSTR lpctszArgs[3] = {
           __FUNCTIONW__,
           _T("�T�[�r�X�̏�ԑJ�ڂɎ��s���܂����B"),
           _T("") // @ToDo �����p�̃��b�Z�[�W���K�v
        };

        _log.Write(EVENTLOG_ERROR_TYPE, CATEGORY_SEVICE_CORE, SVC_ERROR_API, 3, (LPCTSTR*)&lpctszArgs);
    }

    return ret;
}