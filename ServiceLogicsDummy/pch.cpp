//! @file	pch.cpp
//! @brief	プリコンパイル済みヘッダーに対応するソース
//! @note   ここにはpch.hしか配置しません。
//! @note   プリコンパイル済みヘッダーを使用している場合、
//! 		コンパイルを成功させるにはこのソース ファイルが必要です。
//! @author	kumakuma0421@gmail.com
//! @date	2021.4.29

#include "pch.h"

static HANDLE _hEvents[2];
static int event_called = 0;

VOID DebugOut (LPCTSTR lpctszMsg)
{
	TCHAR tszMsg[1024];
	wsprintf (tszMsg, _T ("%s\n"), lpctszMsg);
	::OutputDebugString (tszMsg);
}

BOOL WINAPI StartServiceCtrlDispatcherD (
	_In_ CONST  SERVICE_TABLE_ENTRYW* lpServiceStartTable)
{
	int argc = 0;
	TCHAR** argv = nullptr;

	DebugOut (__FUNCTIONW__);
	DebugOut (lpServiceStartTable->lpServiceName);
	lpServiceStartTable->lpServiceProc (argc, argv);

	return TRUE;
}

BOOL WINAPI StartServiceD (
	_In_            SC_HANDLE            hService,
	_In_            DWORD                dwNumServiceArgs,
	_In_reads_opt_ (dwNumServiceArgs)     LPCWSTR* lpServiceArgVectors)
{
	DebugOut (__FUNCTIONW__);

	return TRUE;
}

SERVICE_STATUS_HANDLE WINAPI RegisterServiceCtrlHandlerD (
	_In_    LPCWSTR                    lpServiceName,
	_In_    __callback
			LPHANDLER_FUNCTION          lpHandlerProc)
{
	DebugOut (__FUNCTIONW__);

	return (SERVICE_STATUS_HANDLE)1;
}

BOOL WINAPI SetServiceStatus (
	_In_        SERVICE_STATUS_HANDLE   hServiceStatus,
	_In_        LPSERVICE_STATUS        lpServiceStatus)
{
	DebugOut (__FUNCTIONW__);

	if (lpServiceStatus->dwCurrentState == SERVICE_RUNNING)
	{
		HANDLE hEvent = nullptr;
		::SetEvent (_hEvents[0]);
		::SetEvent (_hEvents[1]);
	}

	return TRUE;
}

HANDLE WINAPI CreateEventD (
	_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
	_In_ BOOL bManualReset,
	_In_ BOOL bInitialState,
	_In_opt_ LPCWSTR lpName)
{
	HANDLE hEvent = ::CreateEventW (lpEventAttributes, bManualReset, bInitialState, lpName);
	_hEvents[event_called++] = hEvent;
	return hEvent;
}