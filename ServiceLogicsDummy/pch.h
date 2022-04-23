//! @file   pch.h
//! @brief  プリコンパイル済みヘッダー ファイルです。
//! @note   次のファイルは、その後のビルドのビルド パフォーマンスを向上させるため 1 回だけコンパイルされます。
//! 		コード補完や多くのコード参照機能などの IntelliSense パフォーマンスにも影響します。
//! 		ただし、ここに一覧表示されているファイルは、ビルド間でいずれかが更新されると、すべてが再コンパイルされます。
//! 		頻繁に更新するファイルをここに追加しないでください。追加すると、パフォーマンス上の利点がなくなります。
//! @author kumakuma0421@gmail.com
//! @date	2021.4.29

#pragma once

#include "framework.h"

// ----------------------------------------------------------------------------
#undef StartServiceCtrlDispatcher
#define StartServiceCtrlDispatcher StartServiceCtrlDispatcherD

extern BOOL WINAPI StartServiceCtrlDispatcherD (
	_In_ CONST  SERVICE_TABLE_ENTRYW* lpServiceStartTable);

// ----------------------------------------------------------------------------
#undef StartService
#define StartService StartServiceD

extern BOOL WINAPI StartServiceD (
	_In_            SC_HANDLE            hService,
	_In_            DWORD                dwNumServiceArgs,
	_In_reads_opt_ (dwNumServiceArgs)     LPCWSTR* lpServiceArgVectors);

// ----------------------------------------------------------------------------
#undef RegisterServiceCtrlHandler
#define RegisterServiceCtrlHandler RegisterServiceCtrlHandlerD

extern SERVICE_STATUS_HANDLE WINAPI RegisterServiceCtrlHandlerD (
	_In_    LPCWSTR                    lpServiceName,
	_In_    __callback
			LPHANDLER_FUNCTION          lpHandlerProc
);

// ----------------------------------------------------------------------------
#undef SetServiceStatus
#define SetServiceStatus SetServiceStatusD

extern BOOL WINAPI SetServiceStatusD (
	_In_        SERVICE_STATUS_HANDLE   hServiceStatus,
	_In_        LPSERVICE_STATUS        lpServiceStatus
);

// ----------------------------------------------------------------------------
#undef CreateEvent
#define CreateEvent CreateEventD

extern HANDLE WINAPI CreateEventD (
	_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
	_In_ BOOL bManualReset,
	_In_ BOOL bInitialState,
	_In_opt_ LPCWSTR lpName);
