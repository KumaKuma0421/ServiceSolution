//! @file   EventLogger.h
//! @brief  イベントログに書き込むクラス
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "ServiceMessage.h"

//! @brief イベントログ書き込みクラス 
class DECLSPEC EventLogger final
{
public:
    //! @brief コンストラクタ
    EventLogger()
    {
        _hEventSource = nullptr;
    };

    //! @brief デストラクタ
    ~EventLogger()
    {
        if (_hEventSource != nullptr) Exit();
    };

    //! @brief 初期化処理
    //! @param lpctszServiceName サービス名
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Init(LPCTSTR lpctszServiceName);

    //! @brief 終了処理
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Exit();

    //! @brief
    //!   汎用的なログ出力処理
    //! @param wEventType 以下の値を設定します。
    //!   - EVENTLOG_SUCCESS
    //!   - EVENTLOG_ERROR_TYPE
    //!   - EVENTLOG_WARNING_TYPE
    //!   - EVENTLOG_INFORMATION_TYPE
    //! @param wEventCategory 以下の値を設定します。
    //!   - CATEGORY_SERVICE_CORE
    //!   - CATEGORY_SERVICE_MANAGER
    //!   - CATEGORY_SERVICE_CONTROL
    //!   - CATEGORY_SERVICE
    //! @param dwEventID 以下の値を設定します。
    //!   - SVC_SUCCESS_SYSTEM
    //!   - SVC_SUCCESS_RUNTIME
    //!   - SVC_SUCCESS_API
    //!   - SVC_SUCCESS_IO
    //!   - SVC_INFO_SYSTEM
    //!   - SVC_INFO_RUNTIME
    //!   - SVC_INFO_API
    //!   - SVC_INFO_IO
    //!   - SVC_INFO_TRACE
    //!   - SVC_WARNING_SYSTEM
    //!   - SVC_WARNING_RUNTIME
    //!   - SVC_WARNING_API
    //!   - SVC_WARNING_IO
    //!   - SVC_ERROR_SYSTEM
    //!   - SVC_ERROR_RUNTIME
    //!   - SVC_ERROR_API
    //!   - SVC_ERROR_IO
    //! @param wArgc    パラメーターの個数
    //! @note
    //!   可変パラメータはすべて文字列です。
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Log(
        WORD wEventType,
        WORD wEventCategory,
        DWORD dwEventID,
        WORD wArgc,
        ...);

    //! @brief
    //!   APIエラーのログ出力用
    //! @param wCategory 以下の値を設定します。
    //!   - CATEGORY_SERVICE_CORE
    //!   - CATEGORY_SERVICE_MANAGER
    //!   - CATEGORY_SERVICE_CONTROL
    //!   - CATEGORY_SERVICE
    //! @param dwErrorCode  GetLastError()の値 
    //! @param lpctszFunctionName 呼び出し元の関数名
    //! @param lpctszErrorAPI エラーを発生したAPI名
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI ApiError(
        WORD wCategory,
        DWORD dwErrorCode,
        LPCTSTR lpctszFunctionName,
        LPCTSTR lpctszErrorAPI);

    //! @brief
    //!   処理開始ログ出力用
    //! @param wCategory 以下の値を設定します。
    //!   - CATEGORY_SERVICE_CORE
    //!   - CATEGORY_SERVICE_MANAGER
    //!   - CATEGORY_SERVICE_CONTROL
    //!   - CATEGORY_SERVICE
    //! @param lpctszFunctionName   呼び出し元関数名(__FUNCTIONW__)
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI TraceStart(WORD wCategory, LPCTSTR lpctszFunctionName);

    //! @brief
    //!   処理終了ログ出力用
    //! @param wCategory 以下の値を設定します。
    //!   - CATEGORY_SERVICE_CORE
    //!   - CATEGORY_SERVICE_MANAGER
    //!   - CATEGORY_SERVICE_CONTROL
    //!   - CATEGORY_SERVICE
    //! @param lpctszFunctionName   呼び出し元関数名(__FUNCTIONW__)
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI TraceFinish(WORD wCategory, LPCTSTR lpctszFunctionName);
    
    //! @brief
    //!   汎用トレースログ出力用
    //! @param wCategory 以下の値を設定します。
    //!   - CATEGORY_SERVICE_CORE
    //!   - CATEGORY_SERVICE_MANAGER
    //!   - CATEGORY_SERVICE_CONTROL
    //!   - CATEGORY_SERVICE
    //! @param lpctszFunctionName   呼び出し元関数名(__FUNCTIONW__)
    //! @param lpctszMessage        追加の出力メッセージ
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL WINAPI Trace(
        WORD wCategory,
        LPCTSTR lpctszFunctionName,
        LPCTSTR lpctszMessage = nullptr);

private:
    HANDLE _hEventSource;
};
