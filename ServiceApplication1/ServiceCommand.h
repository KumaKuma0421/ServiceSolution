//
// ServiceCommand.h
//

#pragma once

#define COMMAND_INSTALL            _T("/install")
#define COMMAND_REMOVE             _T("/remove")
#define COMMAND_ENABLE             _T("/enable")
#define COMMAND_ONDEMAND           _T("/ondemand")
#define COMMAND_DISABLE            _T("/disable")
#define COMMAND_CHANGE_DESCRIPTION _T("/description")
#define COMMAND_START              _T("/start")
#define COMMAND_STOP               _T("/stop")

#include "ServiceCommon.hpp"

class ServiceCommand final
{
public:
    BOOL Command(LPCTSTR lpctszCommand, LPCTSTR lpctszOption = nullptr);

    BOOL Install();
    BOOL Remove();
    BOOL Enable(BOOL bAuto);
    BOOL Disable();
    BOOL ChangeDescription(LPCTSTR lpctszDescription);
    BOOL Start();
    BOOL Stop();
};

