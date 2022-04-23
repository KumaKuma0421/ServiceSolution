//! @file	ServiceInfo.h
//! @brief  サービス情報クラス
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "pch.h"

#define INITIAL_FILE _T("ServiceConfig.ini")

//! @brief サービス情報クラス
class DECLSPEC ServiceInfo
{
public:
	//! @brief コンストラクタ
	ServiceInfo ();

	//! @brief コンストラクタ
	//! @param lpctszServiceName サービス名
	ServiceInfo (LPCTSTR lpctszServiceName);

	//! @brief 初期化処理
	//! @param lpctszSectionName サービス名
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL Initialize (LPCTSTR lpctszSectionName);

	//! @brief 現在の稼働しているモジュールのあるディレクトリ名を取得
	//! @return 現在の稼働しているモジュールのあるディレクトリ名
	LPCTSTR GetWorkDirectory () { return _tszWorkDirectory; };

	//! @brief サービス名を取得
	//! @return サービス名
	LPCTSTR GetName () { return _tszServiceName; };

	//! @brief サービス表示名を取得
	//! @return サービス表示名
	LPCTSTR GetDisplayName () { return _tszServiceDisplayName; };

	//! @brief サービス説明文を取得
	//! @return サービス説明文
	LPCTSTR GetDescription () { return _tszServiceDescription; };

	//! @brief サービスプログラムファイル名を取得
	//! @return サービスプログラムファイル名 
	LPCTSTR GetExecutePath () { return _tszExecutePath; };

	//! @brief このサービスが依存しているサービス名を取得
	//! @return このサービスが依存しているサービス名
	LPCTSTR GetDependentService () { return _tszDependentService; };

	//! @brief サービスの開始方法を取得
	//! @return サービスの開始方法（自動、手動）
	DWORD GetStartType ()
	{
		return _bAutoStart ? SERVICE_AUTO_START : SERVICE_DEMAND_START;
	};

private:
	TCHAR _tszWorkDirectory[MAX_PATH];
	TCHAR _tszServiceName[MAX_PATH];
	TCHAR _tszServiceDisplayName[MAX_PATH];
	TCHAR _tszServiceDescription[MAX_PATH];
	TCHAR _tszExecutePath[MAX_PATH];
	TCHAR _tszDependentService[MAX_PATH];
	BOOL _bAutoStart;
};