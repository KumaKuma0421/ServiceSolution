//! @file	Main.cpp
//! @brief  �T���v���T�[�r�X ���̂Q
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#include "pch.h"
#include "Main.h"
#include "Service2.h"
#include "ServiceInfo.h"

static EventLogger __logger;
static ServiceCore __core (__logger);

int WINAPI _tmain (int argc, TCHAR** argv)
{
	BOOL ret;

	_tsetlocale (LC_ALL, _T ("Japanese"));

	do
	{
		ServiceInfo si;
		ret = si.Initialize (_T ("Service2"));
		if (!ret)
		{
			_tprintf (_T ("ServiceInfo �̎擾�Ɏ��s���܂����B\n"));
			break;
		}

		ret = __logger.Init (si.GetName ());
		if (!ret)
		{
			_tprintf (_T ("EventLogger �̏������Ɏ��s���܂����B\n"));
			break;
		}

		if (argc > 1)
		{
			ServiceCommand command (si);
			ret = command.Command (argv[1], argv[2]);
		}
		else
		{
			IService* pService = new Service2 (__logger);
			ret = __core.Entry (ServiceMain, CtrlHandler, pService, &si);
		}

		if (!ret) break;

		ret = __logger.Exit ();
		if (!ret) _tprintf (_T ("EventLogger �̏I�������Ɏ��s���܂����B\n"));

	} while (0);

	return ret == TRUE ? ERROR_SUCCESS : -1;
}

VOID WINAPI ServiceMain (DWORD dwArgc, LPTSTR* lptszArgv)
{
	__core.Main (dwArgc, lptszArgv);
}

VOID WINAPI CtrlHandler (DWORD dwControlCode)
{
	__core.Handler (dwControlCode);
}