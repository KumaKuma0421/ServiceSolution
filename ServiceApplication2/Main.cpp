//
// Main.cpp
//

#include "pch.h"
#include "Main.h"
#include "Service1.h"
#include "ServiceInfo.h"

static EventLogger __logger;
static ServiceCore __core(__logger);

//
// SCM����N�����ɌĂ΂�܂��B
// 
// �p�����[�^������ꍇ�́A�R�}���h�N���Ɖ��߂��܂��B
// 
int WINAPI _tmain(int argc, TCHAR** argv)
{
	BOOL ret;

	_tsetlocale(LC_ALL, _T("Japanese"));

	TCHAR tszPath[MAX_PATH] = { 0 };
	DWORD dwRet = ::GetModuleFileName(nullptr, tszPath, MAX_PATH);
	if (!dwRet)
	{
		_tprintf(_T("GetModuleFileName()�����s���܂����B\n"));
		return -1;
	}
	else
	{
		for (int i = (int)_tcslen(tszPath); i >= 0; i--)
		{
			if (tszPath[i] == '\\')
			{
				tszPath[i] = '\0';
				break;
			}
		}
	}

	ServiceInfo si(tszPath, SERVICE_NAME, SERVICE_DISPLAY_NAME, SERVICE_DESCRIPTION);

	do
	{
		ret = __logger.Init(si.GetName());
		if (!ret)
		{
			_tprintf(_T("EventLogger �̏������Ɏ��s���܂����B\n"));
			break;
		}

		if (argc > 1)
		{
			ServiceCommand command(si);
			ret = command.Command(argv[1], argv[2]);
		}
		else
		{
			IService* pService = new Service1(__logger);
			ret = __core.Entry(SvcMain, CtrlHandler, pService, &si);
		}

		if (!ret) break;

		ret = __logger.Exit();
		if (!ret) _tprintf(_T("EventLogger �̏I�������Ɏ��s���܂����B\n"));

	} while (0);

	return ret == TRUE ? ERROR_SUCCESS : -1;
}

//
// SCM����T�[�r�X�J�n���ɌĂ΂�܂��B
//
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lptszArgv)
{
	__core.Main(dwArgc, lptszArgv);
}

//
// SCM�����~�����Ȃǂ̐��䏈���v�����ɌĂ΂�܂��B
//
VOID WINAPI CtrlHandler(DWORD dwControlCode)
{
	__core.Handler(dwControlCode);
}