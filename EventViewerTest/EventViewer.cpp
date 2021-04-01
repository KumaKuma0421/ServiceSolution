//
// EventViewer.cpp
//

#include "pch.h"
#include "EventViewer.h"

EventViewer::EventViewer()
{
	_hEventSource = nullptr;
}

EventViewer::~EventViewer()
{
	if (_hEventSource != nullptr) Exit();
}

BOOL EventViewer::Init(LPCTSTR lpctszServiceName)
{
	_hEventSource = ::RegisterEventSource(nullptr, lpctszServiceName);

	return _hEventSource == nullptr ? FALSE : TRUE;
}

BOOL EventViewer::Exit()
{
	return ::DeregisterEventSource(_hEventSource);
}

BOOL EventViewer::Log(
	WORD wEventType,
	WORD wEventCategory,
	DWORD dwEventID,
	WORD wNumOfArgs,
	LPCTSTR* lpctszArgs)
{
	BOOL ret = FALSE;

	if (nullptr != _hEventSource)
	{
		//! イベントビューアーにメッセージを書き込みます。
		ret = ::ReportEvent(
			_hEventSource,  // event log handle
			wEventType,     // event type
			wEventCategory, // event category
			dwEventID,      // event identifier
			nullptr,        // no security identifier
			wNumOfArgs,     // size of lpszStrings array
			0,              // no binary data
			lpctszArgs,     // array of strings
			nullptr         // no binary data
		);
	}

	return ret;
}

BOOL EventViewer::Open(LPCTSTR lpctszServiceName)
{
	_hEventSource = ::OpenEventLog(
		nullptr,
		lpctszServiceName
	);

	return _hEventSource == nullptr ? FALSE : TRUE;
}

BOOL EventViewer::Close()
{
	return ::CloseEventLog(_hEventSource);
}

BOOL EventViewer::Clear()
{
	return ::ClearEventLog(_hEventSource, nullptr);
}

BOOL EventViewer::Read(DWORD dwReadFlags, DWORD dwRecordOffset, LPVOID lpvBuffer, DWORD& dwBufSize)
{
	DWORD dwBytesRead = 0;
	DWORD dwBytesNeeded = 0;
	BOOL ret;

	ret = ::ReadEventLog(
		_hEventSource,
		dwReadFlags,
		dwRecordOffset,
		lpvBuffer,
		dwBufSize,
		&dwBytesRead,
		&dwBytesNeeded
	);

	return ret;
}