//! @file   Service1.h
//! @brief	サービスを提供するクラス
//! @author	kumakuma0421@gmail.com
//! @date	2021.4.29

#pragma once

#include "Thread.h"
#include "EventLogger.h"
#include "IService.h"
#include "ServiceMessage.h"

//! @brief サービスを提供するクラス
class Service1 : public Thread, public IService
{
public:
	//! @brief コンストラクタ
	//! @param logger EventLogger
	Service1 (EventLogger& logger);

	//! @brief サービスを開始します。
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Start ();

	//! @brief サービスを停止します。
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Suspend ();

	//! @brief サービスを再開します。
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Resume ();

	//! @brief サービスを停止します。
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Stop ();

	//! @brief サービスの終了を待機します。
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Wait ();

private:
	//! @brief 隠蔽されたコンストラクタ
	Service1 ();

	//! @brief サービス開始関数
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	BOOL WINAPI Run ();

	EventLogger& _logger;
};
