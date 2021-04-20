// -----------------------------------------------------------------------------
// ServiceCommand.h
// -----------------------------------------------------------------------------

#pragma once

#define COMMAND_INSTALL                   _T("/install")
#define COMMAND_STATUS                    _T("/status")
#define COMMAND_REMOVE                    _T("/remove")
#define COMMAND_ENABLE                    _T("/enable")
#define COMMAND_ONDEMAND                  _T("/ondemand")
#define COMMAND_DISABLE                   _T("/disable")
#define COMMAND_CHANGE_DELAYED_AUTO_START _T("/delayed")
#define COMMAND_START                     _T("/start")
#define COMMAND_STOP                      _T("/stop")
#define COMMAND_CHANGE_DESCRIPTION        _T("/description")

#include "ServiceCommon.hpp"

typedef BOOL (WINAPI *TemplateMethod)(ServiceControl& sc, LPVOID lpvParam);

class DECLSPEC ServiceCommand final
{
public:
    BOOL WINAPI Command(LPCTSTR lpctszCommand, LPCTSTR lpctszOption = nullptr);

    BOOL WINAPI Install(LPCTSTR lpctszModulePath);
    BOOL WINAPI Status();
    BOOL WINAPI Remove();
    BOOL WINAPI Enable(BOOL bAuto);
    BOOL WINAPI Disable();
    BOOL WINAPI ChangeConfig2Description(LPCTSTR lpctszDescription);
    BOOL WINAPI ChangeConfig2DelayedAutoStart();
    BOOL WINAPI Start();
    BOOL WINAPI Stop();

private:
	BOOL WINAPI TemplateAction(
		DWORD dwOpenParam,
		TemplateMethod method,
		LPCTSTR lpctszLastMessage,
		LPVOID lpvParam = nullptr);
    VOID WINAPI ShowCurrentStatus(QueryResponse response);
    BOOL WINAPI StopDependentServices(
        ServiceControl& sc,
        LPVOID lpvParam);
};

