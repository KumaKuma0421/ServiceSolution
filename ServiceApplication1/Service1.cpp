//! @file   Service1.cpp
//! @brief	サービスを提供するクラス
//! @author	kumakuma0421@gmail.com
//! @date	2021.4.29

#include "pch.h"
#include "Service1.h"

Service1::Service1(EventLogger& logger)
    :_logger(logger)
{
}

BOOL Service1::Start()
{
    return Thread::Start();
}

BOOL Service1::Suspend()
{
    return Thread::Suspend();
}

BOOL Service1::Resume()
{
    return Thread::Resume();
}

BOOL Service1::Stop()
{
    return Thread::Stop();
}

BOOL Service1::Wait()
{
    return Thread::Wait();
}

BOOL Service1::Run()
{
    _logger.TraceStart(CATEGORY_SERVICE, __FUNCTIONW__);
    
    _logger.Log(
        EVENTLOG_SUCCESS,
        CATEGORY_SERVICE,
        SVC_SUCCESS_SYSTEM,
        2,
        __FUNCTIONW__,
        _T("サービスを開始します。"));

    while (TRUE)
    {
        DWORD dwRet = _event.Wait(5000);
        switch (dwRet)
        {
            case WAIT_TIMEOUT:
                _logger.Log(
                    EVENTLOG_INFORMATION_TYPE,
                    CATEGORY_SERVICE,
                    SVC_SUCCESS_API,
                    2,
                    __FUNCTIONW__,
                    _T("サービスを実行中です。"));
                break;

            case WAIT_OBJECT_0:
            case WAIT_ABANDONED_0:
                _logger.Log(
                    EVENTLOG_INFORMATION_TYPE,
                    CATEGORY_SERVICE,
                    SVC_SUCCESS_API,
                    2,
                    __FUNCTIONW__,
                    _T("サービス終了イベントを受信しました。"));
                ::ExitThread(ERROR_SUCCESS);
                break;

            default:
                _logger.Log(
                    EVENTLOG_ERROR_TYPE,
                    CATEGORY_SERVICE,
                    SVC_ERROR_API,
                    2,
                    __FUNCTIONW__,
                    _T("異常なサービス終了イベントを受信しました。"));
                ::ExitThread((DWORD)-1);
                break;
        }
    }

    _logger.Log(
        EVENTLOG_SUCCESS,
        CATEGORY_SERVICE,
        SVC_SUCCESS_SYSTEM,
        2,
        __FUNCTIONW__,
        _T("サービスを終了します。"));

    _logger.TraceFinish(CATEGORY_SERVICE, __FUNCTIONW__);
    return TRUE;
}