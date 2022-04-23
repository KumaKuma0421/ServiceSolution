//! @file   ServiceCommon.hpp
//! @brief  サービス制御クラスを集約したファイル
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "ServiceInfo.h"

#define LOG_ROOT _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\ServiceApplication")

#define ENTRY_MAX_SIZE _T("MaxSize")
#define LOG_MAX_SIZE 0x1400000

#define ENTRY_RETENTION _T("Retention")
#define LOG_RETENTION 0

#define ENTRY_FILE _T("File")
#define LOG_FILE _T("%SystemRoot%\\System32\\Winevt\\Logs\\ServiceApplication.evtx")

#define EVENT_ROOT _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\ServiceApplication")

#define ENTRY_CATEGORY_COUNT _T("CategoryCount")
#define EVENT_CATEGORY_COUNT 4

#define ENTRY_CATEGORY_MESSAGE_FILE _T("CategoryMessageFile")
#define EVENT_CATEGORY_MESSAGE_FILE _T("ServiceMessage.dll")

#define ENTRY_EVENT_MESSAGE_FILE _T("EventMessageFile")
#define EVENT_EVENT_MESSAGE_FILE _T("ServiceMessage.dll")

#define ENTRY_TYPES_SUPPORTED _T("TypesSupported")
#define EVENT_TYPES_SUPPORTED 7

#define ERROR_REGISTRY_REGISTER _T("レジストリの登録に失敗しました。")

#define MAX_MESSAGE_LEN 256
#define MAX_STATE_TRANSITION_TIME 3000

//! @brief メッセージをコンソールに出力します。
//! @param lpctszMessage 出力するメッセージ
//! @param dwErrorCode GetLastError()の出力地
//! @param lpctszOption 追加のメッセージ
//! @return 
VOID WINAPI PrintMessage (
	LPCTSTR lpctszMessage,
	DWORD dwErrorCode = GetLastError (),
	LPCTSTR lpctszOption = nullptr);

//! @brief 手動、自動、無効の選択肢
enum class BootSettings
{
	//! @brief 手動
	OnDemand = SERVICE_DEMAND_START,

	//! @brief 自動
	Auto = SERVICE_AUTO_START,

	//! @brief 無効
	Disable = SERVICE_DISABLED
};

//! @brief サービスの状態遷移
enum class QueryResponse
{
	//! @brief エラー
	Error = -1,

	//! @brief タイムアウト
	Timeout = 0,

	//! @brief 停止
	Stopped = SERVICE_STOPPED,

	//! @brief 開始中
	StartPending = SERVICE_START_PENDING,

	//! @brief 停止中
	StopPending = SERVICE_STOP_PENDING,

	//! @brief 実行中
	Running = SERVICE_RUNNING,

	//! @brief 再開中
	ContinuePending = SERVICE_CONTINUE_PENDING,

	//! @brief 中断中
	PausePending = SERVICE_PAUSE_PENDING,

	//! @brief 中断
	Paused = SERVICE_PAUSED
};

//! @brief SC_HANDLERを扱うクラスのスーパークラス
class DECLSPEC ServiceHandler
{
public:
	//! @brief コンストラクタ
	ServiceHandler ()
	{
		_handle = nullptr;
	};

	//! @brief デストラクタ
	virtual ~ServiceHandler ()
	{
		if (_handle != nullptr) Close ();
	};

	//! @brief  クローズ処理
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	virtual BOOL WINAPI Close ()
	{
		return ::CloseServiceHandle (_handle);
	}

	//! @brief  ハンドル取得
	//! @return SC_HANDLE
	virtual SC_HANDLE WINAPI GetHandle () { return _handle; };

protected:
	SC_HANDLE _handle;
};

//! @brief サービスコントロールマネージャ(SCM)の操作用クラス
class DECLSPEC ServiceControlManager final : public ServiceHandler
{
public:
	//! @brief  SCMのオープン
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Open ()
	{
		_handle = ::OpenSCManager (
			nullptr,
			nullptr,
			SC_MANAGER_ALL_ACCESS);

		return _handle == nullptr ? FALSE : TRUE;
	};
};

//! @brief 個別のサービスを管理するクラス
class DECLSPEC ServiceControl final : public ServiceHandler
{
public:
	//! @brief コンストラクタ
	//! @param manager サービスマネージャクラス
	//! @param si 個別のサービスに関する情報
	ServiceControl (ServiceControlManager & manager, ServiceInfo & si)
		: _manager (manager), _si (si)
	{};

	//! @brief サービスの作成
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Create ()
	{
		_handle = ::CreateService (
			_manager.GetHandle (),       // SCM database 
			_si.GetName (),              // name of service 
			_si.GetDisplayName (),       // service name to display 
			SERVICE_ALL_ACCESS,         // desired access 
			SERVICE_WIN32_OWN_PROCESS,  // service type 
			_si.GetStartType (),         // start type 
			SERVICE_ERROR_NORMAL,       // error control type 
			_si.GetExecutePath (),       // path to service's binary 
			nullptr,                    // no load ordering group 
			nullptr,                    // no tag identifier 
			_si.GetDependentService (),  // dependent service
			nullptr,                    // LocalSystem account 
			nullptr);                   // no password 

		return _handle == nullptr ? FALSE : TRUE;
	};

	//! @brief サービスのオープン
	//! @param dwDesiredAccess オープンの際に必要なアクセス権
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Open (DWORD dwDesiredAccess)
	{
		_handle = ::OpenService (
			_manager.GetHandle (),
			_si.GetName (),
			dwDesiredAccess);

		return _handle == nullptr ? FALSE : TRUE;
	};

	//! @brief サービスの設定情報取得
	//! @param lpQueryServiceConfig サービス設定情報
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI QueryConfig (LPQUERY_SERVICE_CONFIG& lpQueryServiceConfig)
	{
		BOOL ret;
		DWORD dwBytesNeeded = 0;

		ret = ::QueryServiceConfig (_handle, nullptr, 0, &dwBytesNeeded);
		if (!ret && dwBytesNeeded)
		{
			lpQueryServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc (LPTR, dwBytesNeeded);
			ret = ::QueryServiceConfig (_handle, lpQueryServiceConfig, dwBytesNeeded, &dwBytesNeeded);
		}

		return ret;
	}

	//! @brief サービスの状態取得
	//! @param status サービスの状態
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI QueryStatusEx (SERVICE_STATUS_PROCESS& status)
	{
		DWORD dwBytesNeeded;

		return ::QueryServiceStatusEx (
			_handle,                        // handle to service 
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE)&status,                // address of structure
			sizeof (SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded);                // size needed if buffer is too small
	}

	//! @brief サービスの状態取得
	//! @param wait 遷移中の場合、待機するステータス
	//! @return QueryResponse
	QueryResponse WINAPI QueryStatus (QueryResponse wait);

	//! @brief サービスの開始
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Start ()
	{
		return ::StartService (_handle, 0, nullptr);
	}

	//! @brief サービスの停止
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Stop ()
	{
		SERVICE_STATUS service_status = { 0, 0, 0, 0, 0, 0, 0 };

		return ::ControlService (_handle, SERVICE_CONTROL_STOP, &service_status);
	}

	//! @brief サービスの設定変更
	//! @param bootType BootSettings
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI ChangeConfig (BootSettings bootType)
	{
		return ::ChangeServiceConfig (
			_handle,           // handle of service 
			SERVICE_NO_CHANGE, // service type: no change 
			(DWORD)bootType,   // service start type 
			SERVICE_NO_CHANGE, // error control: no change 
			nullptr,           // binary path: no change 
			nullptr,           // load order group: no change 
			nullptr,           // tag ID: no change 
			nullptr,           // dependencies: no change 
			nullptr,           // account name: no change 
			nullptr,           // password: no change 
			nullptr);          // display name: no change
	};

	//! @brief サービスの説明文章の変更
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI ChangeConfig2Description ()
	{
		SERVICE_DESCRIPTION service_description = { (LPTSTR)_si.GetDescription () };

		return ::ChangeServiceConfig2 (
			_handle,
			SERVICE_CONFIG_DESCRIPTION,
			&service_description);
	}

	//! @brief サービスの遅延開始設定
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	//! @note   自動（遅延開始）について
	//!   通常の自動開始サービスの約２分後に開始されるもの。
	//!   自動→自動(遅延開始)なので、事前に自動になっていることが前提。
	//! @sa https://togarasi.wordpress.com/2008/05/17/%E3%82%B5%E3%83%BC%E3%83%93%E3%82%B9%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%A0%E3%81%AE%E8%87%AA%E5%8B%95%E8%B5%B7%E5%8B%95%E3%82%92%E9%81%85%E5%BB%B6%E5%AE%9F%E8%A1%8C%E3%81%AB%E5%A4%89%E6%9B%B4/
	BOOL WINAPI ChangeConfig2DelayedAutoStart (BOOL bDelayed = TRUE)
	{
		SERVICE_DELAYED_AUTO_START_INFO service_delayed_auto_start_info = { bDelayed };

		return ::ChangeServiceConfig2 (
			_handle,
			SERVICE_CONFIG_DELAYED_AUTO_START_INFO,
			&service_delayed_auto_start_info);
	}

	//! @brief サービスの削除
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Delete ()
	{
		return ::DeleteService (_handle);
	};

	//! @brief このサービスに紐づくSCMのハンドルを取得
	//! @return ServiceControlManagerクラス
	ServiceControlManager& GetManager () { return _manager; }

private:
	//! @brief 隠蔽されたコンストラクタ
	ServiceControl ();

	ServiceControlManager& _manager;
	ServiceInfo& _si;
};

//! @brief サービス制御関数の登録を行うクラス
class DECLSPEC ServiceStatusHandler final
{
public:
	//! @brief コンストラクタ
	ServiceStatusHandler ()
	{
		_hService = nullptr;
	};

	//! @brief デストラクタ
	~ServiceStatusHandler ()
	{}

	//! @brief サービス制御関数の登録処理
	//! @param lpctszServiceName サービス名
	//! @param lpfnCtrlHandler サービス制御関数
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Init (LPCTSTR lpctszServiceName, LPHANDLER_FUNCTION lpfnCtrlHandler)
	{
		_hService = ::RegisterServiceCtrlHandler (lpctszServiceName, lpfnCtrlHandler);
		return _hService == nullptr ? FALSE : TRUE;
	};

	//! @brief サービス制御関数登録時のハンドルを取得
	//! @return SERVICE_STATUS_HANDLEハンドル
	SERVICE_STATUS_HANDLE WINAPI GetHandle () { return _hService; };

private:
	SERVICE_STATUS_HANDLE _hService;
};

//! @brief サービス状態の操作クラス
class DECLSPEC ServiceStatus final
{
public:
	//! @brief コンストラクタ
	ServiceStatus ()
	{
		_Status = { 0,0,0,0,0,0,0 };
	};

	//! @brief                  サービスの現在の状態を設定
	//! @param handler          ServiceStatusHandler
	//! @param dwCurrentState   これから遷移させるステータス
	//!   - SERVICE_STOPPED(1)
	//!   - SERVICE_START_PENDING(2)
	//!   - SERVICE_STOP_PENDING(3)
	//!   - SERVICE_RUNNING(4)
	//!   - SERVICE_CONTINUE_PENDING(5)
	//!   - SERVICE_PAUSE_PENDING(6)
	//!   - SERVICE_PAUSED(7)
	//! @param dwWin32ExitCode
	//!   サービスが開始または停止しているときに発生するエラーを
	//!   報告するために使用するエラーコード
	//! @param dwWaitHint
	//!   保留中の開始、停止、一時停止、または継続の操作に必要な
	//!   推定時間 (ミリ秒単位)
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI ReportStatus (
		ServiceStatusHandler handler,
		DWORD dwCurrentState,
		DWORD dwWin32ExitCode,
		DWORD dwWaitHint);

	//! @brief サービスの状態取得
	//! @return SERVICE_STATUS構造体
	SERVICE_STATUS WINAPI GetStatus () { return _Status; };

private:
	SERVICE_STATUS _Status;
};
