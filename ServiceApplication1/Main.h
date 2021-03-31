#pragma once

#include "Service.h"
#include "EventViewer.h"
#include "ServiceMessage.h"

VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lptszArgv);
VOID WINAPI CtrlHandler(DWORD dwControlCode);
