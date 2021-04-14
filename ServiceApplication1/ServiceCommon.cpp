//
// ServiceCommon.cpp
//

#include "pch.h"
#include "ServiceCommon.hpp"

QueryResponse ServiceControl::Query(QueryResponse wait)
{
    QueryResponse response = QueryResponse::Error;
    BOOL ret;
    SERVICE_STATUS_PROCESS status{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    DWORD dwBytesNeeded;

    DWORD dwWaitTime = 0;
#pragma warning(disable:28159)
    DWORD dwStartTickCount = GetTickCount();
#pragma warning(default:28159)
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

    } while (response == wait);

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
