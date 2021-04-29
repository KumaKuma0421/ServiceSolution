//! @file   ServiceCommand.h
//! @brief  サービスに対する操作を集約したクラス
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "ServiceCommon.hpp"
#include "ServiceInfo.h"

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

//! @brief TemplateAction()内部で使用する関数ポインタ
typedef BOOL (WINAPI *TemplateMethod)(ServiceControl& sc, LPVOID lpvParam);

//! @brief サービスに対する操作を集約したクラス
class DECLSPEC ServiceCommand final
{
public:
	//! @brief コンストラクタ
	//! @param si サービス情報クラス
	ServiceCommand(ServiceInfo& si);

    //! @brief                  コマンド処理のエントリー関数
    //! @param lpctszCommand    コマンド名
    //! @param lpctszOption     コマンドオプション
    //! @return                 BOOL
    //! @retval                 TRUE(成功)
    //! @retval                 FALSE(失敗)
    BOOL WINAPI Command(LPCTSTR lpctszCommand, LPCTSTR lpctszOption = nullptr);

    //! @brief  サービスのインストール処理
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Install();

    //! @brief  サービスの状態表示
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Status();

    //! @brief  サービスの削除処理
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Remove();

    //! @brief  自動開始/手動開始を設定
    //! @param  bAuto TRUE(自動開始) FALSE(手動開始)
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Enable(BOOL bAuto);

    //! @brief  サービスの無効化
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Disable();

    //! @brief  サービスの説明文言を変更
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI ChangeConfig2Description();

    //! @brief  自動(遅延）を設定
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI ChangeConfig2DelayedAutoStart();

    //! @brief サービスの開始
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Start();

    //! @brief  サービスの終了
    //! @details
    //!   依存するサービスがある場合、それらを先に終了させます。
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Stop();

private:
	//! @brief 隠蔽されたコンストラクタ
	ServiceCommand();

	//! @brief コマンド操作のプロローグとエピローグ
	//! @param dwOpenParam サービスオープン時のパラメータ
	//! @param method プロローグとエビローグの間で実行する処理
	//! @param lpctszLastMessage 終了直前に出力するメッセージ
	//! @param lpvParam 追加パラメータ
	//! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
	BOOL WINAPI TemplateAction(
		DWORD dwOpenParam,
		TemplateMethod method,
		LPCTSTR lpctszLastMessage,
		LPVOID lpvParam = nullptr);

    //! @brief 現在のサービスステータスを出力します。
    //! @param response 現在のステータス 
    //! @return VOID
    VOID WINAPI ShowCurrentStatus(QueryResponse response);

    //! @brief このサービスに依存しているサービスの停止処理
    //! @param sc 停止させるサービスのServiceControl
    //! @param lpvParam パラメータ
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI StopDependentServices(
        ServiceControl& sc,
        LPVOID lpvParam);
	
    ServiceInfo& _si;
};
