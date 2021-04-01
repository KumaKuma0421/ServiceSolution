//
// Main.cpp
//

#include "pch.h"
#include "Main.h"

static EventLogger __logger;
static ServiceCore __service(__logger);

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
	BOOL ret;

	_tsetlocale(LC_ALL, _T("Japanese"));

	do
	{
		ret = __logger.Init(SERVICE_NAME);
		if (!ret)
		{
			_tprintf(_T("EventLogger initialize failed.\n"));
			break;
		}

		if (argc > 1)
			ret = __service.Command(argv[1]);
		else
			__service.Entry();

		ret = __logger.Exit();
		if (!ret)
			_tprintf(_T("EventLogger closing failed.\n"));

	} while (0);


	return ret == TRUE ? ERROR_SUCCESS : -1;
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