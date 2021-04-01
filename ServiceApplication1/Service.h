//
// Service.h
//

#pragma once

#define STOP_EVENT _T("STOP_MY_SERVICE_EVENT")

#include "Thread.h"

class Service : public Thread
{
public:
    virtual BOOL Run();
};

