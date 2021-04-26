//
// Main.h
//

#pragma once

#include "framework.h"
#include "ServiceCommon.hpp"
#include "ServiceCore.h"
#include "ServiceCommand.h"
#include "EventLogger.h"

#define SERVICE_NAME _T("ServiceApplication1")
#define SERVICE_DISPLAY_NAME _T("サンプル1のサービス")
#define SERVICE_DESCRIPTION _T("これはサンプル1のサービスプログラムです。")

int WINAPI _tmain(int argc, TCHAR** argv);
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lptszArgv);
VOID WINAPI CtrlHandler(DWORD dwControlCode);
