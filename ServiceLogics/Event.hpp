//! @file   Event.hpp
//! @brief	イベントに関するWindowsAPIを集約したクラス
//! @author	kumakuma0421@gmail.com
//! @date	2021.4.29

#pragma once

#include "pch.h"

//! @brief	イベントに関するWindowsAPIを集約したクラス
class DECLSPEC Event final
{
public:
    //! @brief コンストラクタ
    Event()
    {
        _hEvent = nullptr;
    };

    //! デストラクタ
    ~Event()
    {
        if (_hEvent == nullptr) Close();
    }

    //! @brief  イベントを作成します。
    //! @note   手動リセットイベントを作成します。
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Create()
    {
        _hEvent = ::CreateEvent(
            nullptr,
            TRUE,  // manual reset event
            FALSE, // not signaled
            nullptr);

        return _hEvent == nullptr ? FALSE : TRUE;
    };

    //! @brief イベントをオープンします。
    //! @param lpctszEventName イベント名
    //! @return 
    BOOL WINAPI Open(LPCTSTR lpctszEventName)
    {
        _hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpctszEventName);

        return _hEvent == nullptr ? FALSE : TRUE;
    };

    //! @brief  イベントをセット(シグナル状態に)します。
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Set()
    {
        return ::SetEvent(_hEvent);
    };

    //! @brief  イベントをリセット(非シグナル状態に)します。
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Reset()
    {
        return ::ResetEvent(_hEvent);
    };

    //! @brief	シグナル状態になるとブロックを解除します。
    //! @return	ブロック解除の理由
    //! @retval 以下の値が返却されます。
    //!   - ハンドルがシグナル状態(WAIT_OBJECT_O)
    //!   - ハンドルオブジェクトが終了(WAIT_ABANDONED)
    //!   - タイムアウト(WAIT_TIMEOUT)
    //!   - エラー発生(WAIT_FAILED)
    //!   - SetWaitableTimer()の関数呼び出し完了(WAIT_IO_COMPLETION)
    DWORD WINAPI Wait(DWORD dwMilliSeconds = INFINITE)
    {
        return ::WaitForSingleObject(_hEvent, dwMilliSeconds);
    };

    //! @brief	 ハンドルをクローズします。
    //! @return	 BOOL
    //! @retval	 成功(TRUE)
    //! @retval	 失敗(FALSE)
    BOOL WINAPI Close()
    {
        return ::CloseHandle(_hEvent);
    };

private:
    HANDLE _hEvent;
};
