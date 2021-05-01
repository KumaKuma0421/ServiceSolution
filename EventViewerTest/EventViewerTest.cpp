//
// EventViewerTest.cpp
//

#include "pch.h"
#include "EventViewer.h"
#include "..\ServiceMessage\ServiceMessage.h"

#define SVC_NAME _T("ServiceApplication1")

void WriteEventLogTest1()
{
	BOOL ret;
	EventViewer ev;

	LPCTSTR lpctszParams1[] = {
		_T("_tmain()"),
		_T("sample"),
		_T("TEST"),
	};

	// ---------------------

	//ret = ev.Open(_T("Application"));
	//assert(ret);

	//ret = ev.Clear();
	//DWORD dwErr = GetLastError();
	//assert(ret);

	//ret = ev.Close();
	//assert(ret);

	// ---------------------

	ret = ev.Init(SVC_NAME);
	assert(ret);

	ret = ev.Log(
		EVENTLOG_SUCCESS,
		CATEGORY_SERVICE_CORE,
		SVC_SUCCESS_SYSTEM,
		3,
		lpctszParams1);
	assert(ret);

	ret = ev.Log(
		EVENTLOG_SUCCESS,
		CATEGORY_SERVICE_CORE,
		SVC_SUCCESS_RUNTIME,
		3,
		lpctszParams1);
	assert(ret);

	ret = ev.Log(
		EVENTLOG_ERROR_TYPE,
		CATEGORY_SERVICE_MANAGER,
		SVC_ERROR_SYSTEM,
		3,
		lpctszParams1);
	assert(ret);

	ret = ev.Log(
		EVENTLOG_ERROR_TYPE,
		CATEGORY_SERVICE_MANAGER,
		SVC_ERROR_RUNTIME,
		3,
		lpctszParams1);
	assert(ret);

	ret = ev.Log(
		EVENTLOG_WARNING_TYPE,
		CATEGORY_SERVICE_CONTROL,
		SVC_WARNING_SYSTEM,
		3,
		lpctszParams1);
	assert(ret);

	ret = ev.Log(
		EVENTLOG_WARNING_TYPE,
		CATEGORY_SERVICE_CONTROL,
		SVC_WARNING_RUNTIME,
		3,
		lpctszParams1);
	assert(ret);

	ret = ev.Log(
		EVENTLOG_INFORMATION_TYPE,
		CATEGORY_SERVICE_CORE,
		SVC_INFO_SYSTEM,
		3,
		lpctszParams1);
	assert(ret);

	ret = ev.Log(
		EVENTLOG_INFORMATION_TYPE,
		CATEGORY_SERVICE_CORE,
		SVC_INFO_RUNTIME,
		3,
		lpctszParams1);
	assert(ret);

	ret = ev.Exit();
	assert(ret);
}

VOID ReadEventLogTest1()
{
	BOOL ret;
	EventViewer ev;

	DWORD dwReadFlags = EVENTLOG_SEQUENTIAL_READ | EVENTLOG_FORWARDS_READ;
	DWORD dwRecordOffset = 0;
	CONST DWORD cdwRecordSize = sizeof(EVENTLOGRECORD) * 1024; // サンプルは1024でした。
	LPTSTR lptszBuf = new TCHAR[cdwRecordSize];
	DWORD dwBufSize = cdwRecordSize;

	ret = ev.Open(_T("Application"));
	assert(ret);

	while (1)
	{
		ret = ev.Read(dwReadFlags, dwRecordOffset, lptszBuf, dwBufSize);
		if (!ret) break;

		_tprintf(_T("%s"), lptszBuf);
	}

	ret = ev.Close();

	delete[] lptszBuf;

	assert(ret);
}

int _tmain(int argc, TCHAR** argv)
{
	_tsetlocale(LC_ALL, _T("Japanese"));

	WriteEventLogTest1();
	ReadEventLogTest1();

	return ERROR_SUCCESS;
}
