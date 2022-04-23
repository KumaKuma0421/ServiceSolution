//! @file   IService.h
//! @brief  サービス提供プロジェクトへのインターフェースクラス
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "pch.h"

//! @brief  サービス提供プロジェクトへのインターフェースクラス
class IService
{
public:
	//! @brief コンストラクタ
	IService () {};

	//! @brief デストラクタ
	virtual ~IService () {};

	//! @brief 処理開始インターフェース
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	virtual BOOL Start () = 0;

	//! @brief 処理中断インターフェース
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	virtual BOOL Suspend () = 0;

	//! @brief 処理再開インターフェース
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	virtual BOOL Resume () = 0;

	//! @brief 処理終了インターフェース
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	virtual BOOL Stop () = 0;

	//! @brief 処理終了待機インターフェース
	//! @return BOOL
	//! @retval TRUE(成功)
	//! @retval FALSE(失敗)
	virtual BOOL Wait () = 0;
};