//! @file   Service2.cpp
//! @brief	�T�[�r�X��񋟂���N���X
//! @author	kumakuma0421@gmail.com
//! @date	2021.4.29

#include "pch.h"
#include "Service2.h"

Service2::Service2(EventLogger& logger)
    :_logger(logger)
{
}

BOOL Service2::Start()
{
    return Thread::Start();
}

BOOL Service2::Suspend()
{
    return Thread::Suspend();
}

BOOL Service2::Resume()
{
    return Thread::Resume();
}

BOOL Service2::Stop()
{
    return Thread::Stop();
}

BOOL Service2::Wait()
{
    return Thread::Wait();
}

BOOL Service2::Run()
{
    _logger.TraceStart(CATEGORY_SERVICE, __FUNCTIONW__);
    
    _logger.Log(
        EVENTLOG_SUCCESS,
        CATEGORY_SERVICE,
        SVC_SUCCESS_SYSTEM,
        2,
        __FUNCTIONW__,
        _T("�T�[�r�X���J�n���܂��B"));

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
                    _T("�T�[�r�X�����s���ł��B"));
                break;

            case WAIT_OBJECT_0:
            case WAIT_ABANDONED_0:
                _logger.Log(
                    EVENTLOG_INFORMATION_TYPE,
                    CATEGORY_SERVICE,
                    SVC_SUCCESS_API,
                    2,
                    __FUNCTIONW__,
                    _T("�T�[�r�X�I���C�x���g����M���܂����B"));
                ::ExitThread(ERROR_SUCCESS);
                break;

            default:
                _logger.Log(
                    EVENTLOG_ERROR_TYPE,
                    CATEGORY_SERVICE,
                    SVC_ERROR_API,
                    2,
                    __FUNCTIONW__,
                    _T("�ُ�ȃT�[�r�X�I���C�x���g����M���܂����B"));
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
        _T("�T�[�r�X���I�����܂��B"));

    _logger.TraceFinish(CATEGORY_SERVICE, __FUNCTIONW__);
    return TRUE;
}