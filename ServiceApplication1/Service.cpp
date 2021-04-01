//
// Service.cpp
//

#include "pch.h"
#include "Service.h"

BOOL Service::Run()
{
    while (TRUE)
    {
        DWORD dwRet = _event.Wait(1000);
        if (dwRet == WAIT_TIMEOUT)
        {
            // do something.
        }
        else if (WAIT_OBJECT_0)
        {
            // Get stop signal.
            ::ExitThread(ERROR_SUCCESS);
            break;
        }
        else
        {
            ::ExitThread((DWORD)-1);
            break;
        }
    }

    return TRUE;
}