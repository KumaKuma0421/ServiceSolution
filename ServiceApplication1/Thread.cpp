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

BOOL Thread::Start(LPCTSTR lpctszStopEvent)
{
    BOOL ret = FALSE;

    _hThread = ::CreateThread(nullptr, 0, ThreadFunction, this, 0, &_dwThread);
    if (_hThread != nullptr)
    {
        ret = _event.Create(lpctszStopEvent);
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

DWORD Thread::ThreadFunction(LPVOID lpvParam)
{
    Thread* instance = reinterpret_cast<Thread*>(lpvParam);
    return instance->Run();
}