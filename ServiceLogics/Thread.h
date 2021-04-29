//! @file   Thread.h
//! @brief	スレッドに関するWindowsAPIを集約したクラス
//! @author	kumakuma0421@gmail.com
//! @date	2021.4.29

#pragma once

#include "Event.hpp"

//! @brief スレッドに関するWindowsAPIを集約したクラス
class DECLSPEC Thread
{
public:
    //! @brief コンストラクタ
    Thread();

    //! @brief デストラクタ
    virtual ~Thread();

    //! @brief スレッドの起動
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    virtual BOOL Start();

    //! @brief スレッドの停止
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    virtual BOOL Stop();

    //! @brief スレッドの中断
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    virtual BOOL Suspend();

    //! @brief スレッドの再開
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    virtual BOOL Resume();

    //! @brief スレッド処理関数
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    virtual BOOL Run() = 0;

    //! @brief 終了待機
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    virtual BOOL Wait(DWORD dwMillisec = INFINITE);

protected:
    Event _event;

private:
    //! @brief スレッド処理関数
    //! @param lpvParam 実際にスレッド処理を行うインスタンス
    //! @return スレッド関数からの戻り値
    static DWORD ThreadFunction(LPVOID lpvParam);

    HANDLE _hThread;
    DWORD _dwThread;
};