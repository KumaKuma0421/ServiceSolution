//
// Service1.h
//

#pragma once

#include "Thread.h"
#include "EventLogger.h"
#include "IService.h"
#include "ServiceMessage.h"

class Service1 : public Thread, public IService
{
public:
    Service1(EventLogger& logger);

    BOOL Start();
    BOOL Suspend();
    BOOL Resume();
    BOOL Stop();
    BOOL Wait();

private:
    Service1();
    BOOL WINAPI Run();

    EventLogger& _logger;
};

