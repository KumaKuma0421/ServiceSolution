//
// Thread.cpp
//

#include "pch.h"
#include "Thread.h"

Thread::Thread()
{
    _hThread = nullptr;
    _dwThread = 0;
}

#pragma warning( disable : 6258 )
Thread::~Thread()
{
    if (_hThread != nullptr)
    {
        ::TerminateThread(_hThread, (DWORD)-1);
    }
}

BOOL Thread::Start()
{
    BOOL ret = FALSE;

    _hThread = ::CreateThread(nullptr, 0, ThreadFunction, this, 0, &_dwThread);
    if (_hThread != nullptr)
    {
        ret = _event.Create();
    }

    return ret;
}

BOOL Thread::Stop()
{
    return _event.Set();
}

BOOL Thread::Suspend()
{
    BOOL ret;

    DWORD dwRet = ::SuspendThread(_hThread);
    if (dwRet == (DWORD)-1)
        ret = FALSE;
    else
        ret = TRUE;

    return ret;
}

BOOL Thread::Resume()
{
    BOOL ret;

    DWORD dwRet = ::ResumeThread(_hThread);
    if (dwRet == (DWORD)-1)
        ret = FALSE;
    else
        ret = TRUE;

    return ret;
}

BOOL Thread::Wait(DWORD dwMillisec)
{
    BOOL ret;

    DWORD dwRet = ::WaitForSingleObject(_hThread, dwMillisec);
    switch (dwRet)
    {
        case WAIT_TIMEOUT:
            ret = FALSE;
            break;

        case WAIT_OBJECT_0:
        case WAIT_ABANDONED_0:
            ret = TRUE;
            break;

        default:
            ret = FALSE;
            break;
    }

    return ret;
}

// ------------
// PRIVATE ZONE
// ------------

DWORD Thread::ThreadFunction(LPVOID lpvParam)
{
    Thread* instance = reinterpret_cast<Thread*>(lpvParam);
    return instance->Run();
}