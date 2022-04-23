//! @file   ServiceCore.h
//! @brief  サービス実行クラス
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "ServiceCommon.hpp"
#include "Event.hpp"
#include "EventLogger.h"
#include "IService.h"
#include "ServiceInfo.h"

//! @brief サービス実行クラス
class DECLSPEC ServiceCore final
{
public:
	//! @brief コンストラクタ
	//! @param logger EventLoggerクラス
	ServiceCore (EventLogger& logger);

	//! @brief　デストラクタ
	~ServiceCore ();

	//! @brief  main()からの呼び出しに対応する関数
	//! @param fnServiceMain    サービスメイン関数
	//! @param fnHandler        サービス制御関数
	//! @param pService         提供サービスクラス
	//! @param si               サービスに関数情報構造体
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Entry (
		LPSERVICE_MAIN_FUNCTION fnServiceMain,
		LPHANDLER_FUNCTION fnHandler,
		IService* pService,
		ServiceInfo* si);

	//! @brief サービスメイン関数からの呼び出しに対応する関数
	//! @param dwArgc       サービスコール時の引数の数
	//! @param lptszArgv    サービスコール時の引数
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	VOID WINAPI Main (DWORD dwArgc, LPTSTR* lptszArgv);

	//! @brief サービス制御関数からの呼び出しに対応する関数
	//! @param dwControlCode 制御コード
	//! @return VOID
	VOID WINAPI Handler (DWORD dwControlCode);

private:
	//! @brief 初期化処理
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Init ();

	//! @brief 開始処理
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Start ();

	//! @brief 停止処理
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Stop ();

	//! @brief 中断処理
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Suspend ();

	//! @brief 再開処理
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Resume ();

	//! @brief 終了処理
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Exit ();

	//! @brief サービスの現在の状態を設定
	//! @param dwCurrentState   これから遷移させるステータス
	//!   - SERVICE_STOPPED(1)
	//!   - SERVICE_START_PENDING(2)
	//!   - SERVICE_STOP_PENDING(3)
	//!   - SERVICE_RUNNING(4)
	//!   - SERVICE_CONTINUE_PENDING(5)
	//!   - SERVICE_PAUSE_PENDING(6)
	//!   - SERVICE_PAUSED(7)
	//! @param dwWin32ExitCode
	//!   サービスが開始または停止しているときに
	//!   発生するエラーを報告するために使用する
	//!   エラーコード
	//! @param dwWaitHint
	//!   保留中の開始、停止、一時停止、または継続の
	//!   操作に必要な推定時間 (ミリ秒単位)
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI ReportStatus (
		DWORD dwCurrentState,
		DWORD dwWin32ExitCode = NO_ERROR,
		DWORD dwWaitHint = MAX_STATE_TRANSITION_TIME);

	//! @brief サービス制御関数からの制御コード
	DWORD _dwControlCode;

	ServiceStatusHandler _handler;
	ServiceStatus _status;

	Event _event;
	EventLogger& _logger;

	//! @brief サービス制御関数
	LPHANDLER_FUNCTION _fnHandler;

	IService* _pService;
	ServiceInfo* _pSI;
};

