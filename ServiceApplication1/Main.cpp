﻿//
// Main.cpp
//

#include "pch.h"
#include "Main.h"

static EventLogger __logger;
static ServiceCore __core(__logger);

//
// SCMから起動時に呼ばれます。
// 
// パラメータがある場合は、コマンド起動と解釈します。
// 
int WINAPI _tmain(int argc, TCHAR** argv)
{
	BOOL ret;

	_tsetlocale(LC_ALL, _T("Japanese"));

	do
	{
		ret = __logger.Init(MY_SERVICE_NAME);
		if (!ret)
		{
			_tprintf(_T("EventLogger の初期化に失敗しました。\n"));
			break;
		}

		if (argc > 1)
		{
			ServiceCommand command;
			ret = command.Command(argv[1], argv[2]);
		}
		else
		{
			ret = __core.Entry(SvcMain, CtrlHandler);
		}

		if (!ret) break;

		ret = __logger.Exit();
		if (!ret) _tprintf(_T("EventLogger の終了処理に失敗しました。\n"));

	} while (0);

	return ret == TRUE ? ERROR_SUCCESS : -1;
}

//
// SCMからサービス開始時に呼ばれます。
//
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lptszArgv)
{
	__core.Main(dwArgc, lptszArgv);
}

//
// SCMから停止処理などの制御処理要求時に呼ばれます。
//
VOID WINAPI CtrlHandler(DWORD dwControlCode)
{
	__core.Handler(dwControlCode);
}