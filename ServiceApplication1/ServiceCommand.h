//
// ServiceCommand.h
//

#pragma once

#define COMMAND_INSTALL                   _T("/install")
#define COMMAND_REMOVE                    _T("/remove")
#define COMMAND_ENABLE                    _T("/enable")
#define COMMAND_ONDEMAND                  _T("/ondemand")
#define COMMAND_DISABLE                   _T("/disable")
#define COMMAND_CHANGE_DELAYED_AUTO_START _T("/delayed")
#define COMMAND_START                     _T("/start")
#define COMMAND_STOP                      _T("/stop")
#define COMMAND_CHANGE_DESCRIPTION        _T("/description")

#include "ServiceCommon.hpp"

BOOL (*TemplateMethod)(LPVOID lpvParam);

class ServiceCommand final
{
public:
    BOOL Command(LPCTSTR lpctszCommand, LPCTSTR lpctszOption = nullptr);

    BOOL Install(LPCTSTR lpctszModulePath);
    BOOL Remove();
    BOOL Enable(BOOL bAuto);
    BOOL Disable();
    BOOL ChangeConfig2Description(LPCTSTR lpctszDescription);
    BOOL ChangeConfig2DelayedAutoStart();
    BOOL Start();
    BOOL Stop();

private:
    BOOL TemplateAction();
};

