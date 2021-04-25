//
// IService.h
//

#pragma once

#include "pch.h"

class IService
{
public:
    IService() {};
    virtual ~IService() {};

    virtual BOOL Start() = 0;
    virtual BOOL Suspend() = 0;
    virtual BOOL Resume() = 0;
    virtual BOOL Stop() = 0;
    virtual BOOL Wait() = 0;
};