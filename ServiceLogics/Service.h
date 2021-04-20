//
// Service.h
//

#pragma once

#include "Thread.h"

class DECLSPEC Service : public Thread
{
public:
    Service(EventLogger& logger);
    virtual BOOL WINAPI Run();

private:
    Service();
};

