#pragma once

#include "ServiceCore.h"
#include "EventLogger.h"
#include "ServiceMessage.h"

VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lptszArgv);
VOID WINAPI CtrlHandler(DWORD dwControlCode);
