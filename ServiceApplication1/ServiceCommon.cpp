//
// ServiceCommon.cpp
//

#include "pch.h"
#include "ServiceCommon.hpp"

QueryResponse ServiceControl::QueryStatus(QueryResponse wait)
{
    QueryResponse response = QueryResponse::Error;
    BOOL ret;
    SERVICE_STATUS_PROCESS status = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#pragma warning(disable:28159)
    CONST DWORD cdwStartTime = GetTickCount();
#pragma warning(default:28159)
    CONST DWORD cdwTimeout = 30000; // 30-second time-out

    DWORD dwWaitTime = 0;
#pragma warning(disable:28159)
    DWORD dwStartTickCount = GetTickCount();
#pragma warning(default:28159)
    DWORD dwCheckPointBefore = 0;
    BOOL bChecking = FALSE;

    do
    {
        dwWaitTime = status.dwWaitHint / 10;
        if (dwWaitTime < 1000) dwWaitTime = 1000;
        else if (dwWaitTime > 10000) dwWaitTime = 10000;

        Sleep(dwWaitTime);

        ret = QueryService(status);
        if (ret == FALSE)
        {
            response = QueryResponse::Error;
            break;
        }
        else
        {
            // SERVICE_STOPPED              ��~
            // SERVICE_START_PENDING        �J�n��
            // SERVICE_STOP_PENDING         ��~��
            // SERVICE_RUNNING              �J�n
            // SERVICE_CONTINUE_PENDING     �ĊJ��
            // SERVICE_PAUSE_PENDING        ���f��
            // SERVICE_PAUSED               ���f
            // ��~���̏ꍇ�A�ҋ@���Ē�~���^�C���A�E�g�ɂȂ�̂�҂��܂��B
            // �J�n���̏ꍇ�A�ҋ@���ĊJ�n���^�C���A�E�g�ɂȂ�̂�҂��܂��B
            response = static_cast<QueryResponse>(status.dwCurrentState);
            if (response != wait) break;
        }

        if (!bChecking) _tprintf(_T("�T�[�r�X��Ԃ��m�F���ł��B"));
        bChecking = TRUE;

        if (status.dwCheckPoint > dwCheckPointBefore)
        {
            // Continue to wait and check.
#pragma warning(disable:28159)
            dwStartTickCount = GetTickCount();
#pragma warning(default:28159)
            dwCheckPointBefore = status.dwCheckPoint;
        }
        else
        {
#pragma warning(disable:28159)
            if (GetTickCount() - dwStartTickCount > status.dwWaitHint)
#pragma warning(default:28159)
            {
                response = QueryResponse::Timeout;
                break;
            }
        }

#pragma warning(disable:28159)
        if (GetTickCount() - cdwStartTime > cdwTimeout)
#pragma warning(default:28159)
        {
            response = QueryResponse::Timeout;
            break;
        }

        _tprintf(_T("."));

    } while (response == wait);

    if (bChecking) _tprintf(_T("\n"));

    return response;
}

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
