//
// Main.cpp
//

#include "pch.h"
#include "Main.h"

static ServiceCore __service;

//
// SCMから起動時に呼ばれます。
// 
// 手動でパラメータ"/install"付きで呼び出した場合は
// サービスのインストールを行います。
// 
// 手動でパラメータ"/remove"付きで呼び出した場合は
// サービスのアンインストールを行います。
//
int _tmain(int argc, TCHAR** argv)
{
	_tsetlocale(LC_ALL, _T("Japanese"));

	BOOL ret;

	if (argc > 1)
		ret = __service.Command(argv[1]);
	else
		__service.Entry();

	return ERROR_SUCCESS;
}

//
// SCMからサービス開始時に呼ばれます。
//
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lptszArgv)
{
	__service.Main(dwArgc, lptszArgv);
}

//
// SCMから停止処理などの制御処理要求時に呼ばれます。
//
VOID WINAPI CtrlHandler(DWORD dwControlCode)
{
	__service.Handler(dwControlCode);
}