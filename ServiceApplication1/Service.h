//
// Service.h
//

#pragma once

#include "Thread.h"

class Service : public Thread
{
public:
    Service(EventLogger& logger);
    virtual BOOL Run();

private:
    Service();
};

