//! @file   Main.h
//! @brief  サンプルサービス その２
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "framework.h"
#include "ServiceCommon.hpp"
#include "ServiceCore.h"
#include "ServiceCommand.h"
#include "EventLogger.h"

#define SERVICE_NAME _T("ServiceApplication2")
#define SERVICE_DISPLAY_NAME _T("サンプル2のサービス")
#define SERVICE_DESCRIPTION _T("これはサンプル2のサービスプログラムです。")

//! @brief
//!   メイン関数
//! @details
//!   ＯＳからのサービス起動も、コマンドプロンプトからのサービス制御も
//!   この関数から開始します。
//! @param argc 引数の数
//! @param argv 引数
//! @return BOOL
//! @retval ERROR_SUCCESS(成功)
//! @retval -1(失敗)
int WINAPI _tmain(int argc, TCHAR** argv);

//! @brief
//!   サービス関数
//! @details
//!   ＯＳからのサービス開始要求は、ここからの開始となります。
//!   main関数でのStartServiceCtrlDispatcher関数コールを契機に
//!   呼び出されます。
//! @param dwArgc 引数の数
//! @param lptszArgv 引数
//! @return VOID
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lptszArgv);

//! @brief
//!   サービス制御関数
//! @details
//!   ＯＳからのサービス制御要求に対応します。
//! @param dwControlCode
//!   - SERVICE_CONTROL_STOP
//!   - SERVICE_CONTROL_SHUTDOWN
//!   - SERVICE_CONTROL_PAUSE
//!   - SERVICE_CONTROL_PRESHUTDOWN
//!   - SERVICE_CONTROL_CONTINUE
//! @return VOID
VOID WINAPI CtrlHandler(DWORD dwControlCode);
