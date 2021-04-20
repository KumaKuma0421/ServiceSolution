// -----------------------------------------------------------------------------
// ServiceCommand.cpp
// -----------------------------------------------------------------------------

#include "pch.h"
#include "ServiceCommand.h"

BOOL ServiceCommand::Command(LPCTSTR lpctszCommand, LPCTSTR lpctszOption)
{
	BOOL ret = TRUE;

	if (_tcscmp(lpctszCommand, COMMAND_INSTALL) == 0)
	{
		TCHAR tszPath[MAX_PATH] = { 0 };
		DWORD dwRet = ::GetModuleFileName(nullptr, tszPath, MAX_PATH);
		if (!dwRet)
		{
			PrintMessage(_T("GetModuleFileName()"));
		}
		else
			Install(tszPath);
	}
	else if (_tcscmp(lpctszCommand, COMMAND_STATUS) == 0)
	{
		Status();
	}
	else if (_tcscmp(lpctszCommand, COMMAND_REMOVE) == 0)
	{
		Remove();
	}
	else if (_tcscmp(lpctszCommand, COMMAND_ENABLE) == 0)
	{
		Enable(TRUE);
	}
	else if (_tcscmp(lpctszCommand, COMMAND_ONDEMAND) == 0)
	{
		Enable(FALSE);
	}
	else if (_tcscmp(lpctszCommand, COMMAND_DISABLE) == 0)
	{
		Disable();
	}
	else if (_tcscmp(lpctszCommand, COMMAND_CHANGE_DESCRIPTION) == 0)
	{
		ChangeConfig2Description(lpctszOption);
	}
	else if (_tcscmp(lpctszCommand, COMMAND_CHANGE_DELAYED_AUTO_START) == 0)
	{
		ChangeConfig2DelayedAutoStart();
	}
	else if (_tcscmp(lpctszCommand, COMMAND_START) == 0)
	{
		Start();
	}
	else if (_tcscmp(lpctszCommand, COMMAND_STOP) == 0)
	{
		Stop();
	}
	else
	{
		PrintMessage(_T("�s���ȃR�}���h�ł��B"), NO_ERROR, lpctszCommand);
		ret = FALSE;
	}

	return ret;
}

BOOL ServiceCommand::Install(LPCTSTR lpctszModulePath)
{
	auto InstallAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret = sc.ChangeConfig2Description(MY_SERVICE_DESCRIPTION);
		if (!ret)
		{
			PrintMessage(_T("ChangeServiceConfig2()"));
		}

		return ret;
	};

	return TemplateAction(
		0,
		InstallAction,
		_T("�T�[�r�X���C���X�g�[�����܂����B"),
		(LPVOID)lpctszModulePath);
}

BOOL ServiceCommand::Status()
{
	auto StatusAction = [](ServiceControl& sc, void* lpvParam)
	{
		SERVICE_STATUS_PROCESS status = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		BOOL ret = sc.QueryService(status);
		if (!ret)
		{
			PrintMessage(_T("QueryServiceStatusEx()"));
		}
		else
		{
			_tprintf(_T("ServiceType:\n"));
			if (status.dwServiceType & SERVICE_KERNEL_DRIVER)
				_tprintf(_T("  KERNEL_DRIVER\n"));
			else if (status.dwServiceType & SERVICE_FILE_SYSTEM_DRIVER)
				_tprintf(_T("  FILE_SYSTEM_DRIVER\n"));
			else if (status.dwServiceType & SERVICE_WIN32_OWN_PROCESS)
				_tprintf(_T("  WIN32_OWN_PROCESS\n"));
			else if (status.dwServiceType & SERVICE_WIN32_SHARE_PROCESS)
				_tprintf(_T("  WIN32_SHARE_PROCESS\n"));
			else
				_tprintf(_T("  Unknown\n"));
			if (status.dwServiceType & SERVICE_INTERACTIVE_PROCESS)
				_tprintf(_T("  SERVICE_INTERACTIVE_PROCESS\n")); // can be combined.

			_tprintf(_T("CurrentState:\n"));
			switch (status.dwCurrentState)
			{
				case SERVICE_STOPPED:
					_tprintf(_T("  STOPPED\n"));
					break;

				case SERVICE_START_PENDING:
					_tprintf(_T("  START_PENDING\n"));
					break;

				case SERVICE_STOP_PENDING:
					_tprintf(_T("  STOP_PENDING\n"));
					break;

				case SERVICE_RUNNING:
					_tprintf(_T("  RUNNING\n"));
					break;

				case SERVICE_CONTINUE_PENDING:
					_tprintf(_T("  CONTINUE_PENDING\n"));
					break;

				case SERVICE_PAUSE_PENDING:
					_tprintf(_T("  PAUSE_PENDING\n"));
					break;

				case SERVICE_PAUSED:
					_tprintf(_T("  PAUSED\n"));
					break;

				default:
					_tprintf(_T("  unknown\n"));
					break;
			}

			if (status.dwControlsAccepted > 0)
			{
				_tprintf(_T("ControlsAccepted:\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_STOP)
					_tprintf(_T("  STOP\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_PAUSE_CONTINUE)
					_tprintf(_T("  PAUSE_CONTINUE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_SHUTDOWN)
					_tprintf(_T("  SHUTDOWN\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_PARAMCHANGE)
					_tprintf(_T("  PARAMCHANGE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_NETBINDCHANGE)
					_tprintf(_T("  NETBINDCHANGE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_HARDWAREPROFILECHANGE)
					_tprintf(_T("  HARDWAREPROFILECHANGE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_POWEREVENT)
					_tprintf(_T("  POWEREVENT\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_SESSIONCHANGE)
					_tprintf(_T("  SESSIONCHANGE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_PRESHUTDOWN)
					_tprintf(_T("  PRESHUTDOWN\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_TIMECHANGE)
					_tprintf(_T("  TIMECHANGE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_TRIGGEREVENT)
					_tprintf(_T("  TRIGGEREVENT\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_USER_LOGOFF)
					_tprintf(_T("  USER_LOGOFF\n"));
			}

			_tprintf(_T("Win32ExitCode=%u\n"), status.dwWin32ExitCode);
			_tprintf(_T("ServiceSpecificExitCode=%u\n"), status.dwServiceSpecificExitCode);
			_tprintf(_T("CheckPoint=%u\n"), status.dwCheckPoint);
			_tprintf(_T("WaitHint=%u\n"), status.dwWaitHint);
			_tprintf(_T("ProcessId=%u\n"), status.dwProcessId);
			_tprintf(_T("ServiceFlags=%u\n"), status.dwServiceFlags);
		}

		return ret;
	};

	return TemplateAction(
		SERVICE_QUERY_STATUS,
		StatusAction,
		nullptr);
}

BOOL ServiceCommand::Remove()
{
	auto RemoveAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret = sc.Delete();
		if (!ret)
		{
			PrintMessage(_T("DeleteService()"));
		}

		return ret;
	};

	return TemplateAction(
		DELETE,
		RemoveAction,
		_T("�T�[�r�X���A���C���X�g�[�����܂����B"));
}

BOOL ServiceCommand::Enable(BOOL bAuto)
{
	auto EnableAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret;
		BOOL bAuto = *((LPBOOL)lpvParam);

		do
		{
			ret = sc.ChangeConfig(bAuto == TRUE ? BootSettings::Auto : BootSettings::OnDemand);
			if (!ret)
			{
				PrintMessage(_T("ChangeServiceConfig()"));
				break;
			}

			ret = sc.ChangeConfig2DelayedAutoStart(FALSE);
			if (!ret)
			{
				PrintMessage(_T("ChangeServiceConfig2()"));
				break;
			}
		} while (0);

		return ret;
	};

	LPCTSTR lpctszMsg = bAuto ? _T("�T�[�r�X�������N���ɂ��܂����B") : _T("�T�[�r�X���蓮�N���ɂ��܂����B");

	return TemplateAction(
		SERVICE_CHANGE_CONFIG,
		EnableAction,
		lpctszMsg,
		&bAuto);
}

BOOL ServiceCommand::Disable()
{
	auto DisableAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret;

		do
		{
			ret = sc.ChangeConfig(BootSettings::Disable);
			if (!ret)
			{
				PrintMessage(_T("ChangeServiceConfig()"));
				break;
			}

			ret = sc.ChangeConfig2DelayedAutoStart(FALSE);
			if (!ret)
			{
				PrintMessage(_T("ChangeServiceConfig2()"));
				break;
			}
		} while (0);

		return ret;
	};

	return TemplateAction(
		SERVICE_CHANGE_CONFIG,
		DisableAction,
		_T("�T�[�r�X�𖳌��ɂ��܂����B"));
}

BOOL ServiceCommand::ChangeConfig2Description(LPCTSTR lpctszDescription)
{
	auto DescriptionAction = [](ServiceControl& sc, void* lpvParam)
	{
		LPCTSTR lpctszDescription = (LPCTSTR)lpvParam;

		BOOL ret = sc.ChangeConfig2Description(lpctszDescription);
		if (!ret)
		{
			PrintMessage(_T("ChangeServiceConfig2()"));
		}

		return ret;
	};

	return TemplateAction(
		SERVICE_CHANGE_CONFIG,
		DescriptionAction,
		_T("�T�[�r�X�̐�����ύX���܂����B"),
		(LPVOID)lpctszDescription);
}

BOOL ServiceCommand::ChangeConfig2DelayedAutoStart()
{
	auto DelayedAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret = sc.ChangeConfig2DelayedAutoStart();
		if (!ret)
		{
			PrintMessage(_T("ChangeServiceConfig2()"));
		}

		return ret;
	};

	return TemplateAction(
		SERVICE_CHANGE_CONFIG,
		DelayedAction,
		_T("�T�[�r�X�������N��(�x��)�ɂ��܂����B"));
}

BOOL ServiceCommand::Start()
{
	auto StartAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret = FALSE;
		LPCTSTR lpctszMsg = nullptr;
		ServiceCommand* me = (ServiceCommand*)lpvParam;

		QueryResponse response = sc.QueryStatus(QueryResponse::StopPending);

		do
		{
			me->ShowCurrentStatus(response);

			if (response == QueryResponse::Stopped)
			{
				ret = sc.Start();
				if (!ret)
				{
					PrintMessage(_T("StartService()"));
					break;
				}

				response = sc.QueryStatus(QueryResponse::StartPending);
				if (response == QueryResponse::Running)
				{
					PrintMessage(_T("�T�[�r�X���J�n���܂����B"), NO_ERROR);
				}
				else
				{
					PrintMessage(_T("�T�[�r�X�J�n�Ɏ��s���܂����B"), NO_ERROR);
				}
			}
		} while (0);

		return ret;
	};

	return TemplateAction(
		SERVICE_ALL_ACCESS,
		StartAction,
		nullptr,
		this);
}

BOOL ServiceCommand::Stop()
{
	auto StopAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret = FALSE;
		ServiceCommand* me = (ServiceCommand*)lpvParam;

		QueryResponse response = sc.QueryStatus(QueryResponse::StopPending);

		do
		{
			me->ShowCurrentStatus(response);

			if (response == QueryResponse::StartPending ||
				response == QueryResponse::Running ||
				response == QueryResponse::ContinuePending)
			{
				ret = sc.Stop();
				if (!ret)
				{
					PrintMessage(_T("StartService()"));
					break;
				}

				response = sc.QueryStatus(QueryResponse::StopPending);
				if (response == QueryResponse::Stopped)
				{
					PrintMessage(_T("�T�[�r�X���~���܂����B"), NO_ERROR);
				}
				else
				{
					PrintMessage(_T("�T�[�r�X��~�Ɏ��s���܂����B"), NO_ERROR);
				}
			}
		} while (0);

		return ret;
	};

	return TemplateAction(
		SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS,
		StopAction,
		nullptr,
		this);
}

// ------------
// PRIVATE ZONE
// ------------

BOOL ServiceCommand::TemplateAction(
	DWORD dwOpenParam,
	TemplateMethod method,
	LPCTSTR lpctszLastMessage,
	LPVOID lpvParam)
{
	BOOL ret = FALSE;
	ServiceControlManager scm;
	ServiceControl sc(scm, MY_SERVICE_NAME);

	do
	{
		ret = scm.Open();
		if (!ret)
		{
			PrintMessage(_T("OpenSCManager()"));
			break;
		}

		if (dwOpenParam == 0)
		{
			ret = sc.Create((LPCTSTR)lpvParam);
			if (!ret)
			{
				PrintMessage(_T("CreateService()"));
				break;
			}
		}
		else
		{
			ret = sc.Open(dwOpenParam);
			if (!ret)
			{
				PrintMessage(_T("OpenService()"));
				break;
			}
		}

		ret = method(sc, lpvParam);
		if (!ret) break;

		ret = sc.Close();
		if (!ret)
		{
			PrintMessage(_T("CloseServiceHandle()"));
			break;
		}

		ret = scm.Close();
		if (!ret)
		{
			PrintMessage(_T("CloseServiceHandle()"));
			break;
		}

		if (lpctszLastMessage)
			PrintMessage(lpctszLastMessage);

		ret = TRUE;

	} while (0);

	return ret;
}

VOID ServiceCommand::ShowCurrentStatus(QueryResponse response)
{
	LPCTSTR lpctszMsg = nullptr;

	switch (response)
	{
		case QueryResponse::Error:
			lpctszMsg = _T("�T�[�r�X�̋N���m�F���ɃG���[���������܂����B");
			break;

		case QueryResponse::Timeout:
			lpctszMsg = _T("�T�[�r�X�̋N���m�F���Ƀ^�C���A�E�g���������܂����B");
			break;

		case QueryResponse::Stopped:
			lpctszMsg = _T("�T�[�r�X�͒�~���Ă��܂��B");
			break;

		case QueryResponse::StartPending:
		case QueryResponse::Running:
		case QueryResponse::ContinuePending:
			lpctszMsg = _T("�T�[�r�X�͋N�����Ă��܂��B");
			break;

		case QueryResponse::PausePending:
		case QueryResponse::Paused:
			lpctszMsg = _T("�T�[�r�X�͒��f��Ԃł��B");
			break;

		case QueryResponse::StopPending:
		default:
			lpctszMsg = _T("�����^�C���G���[���������܂����B");
			break;
	}

	PrintMessage(lpctszMsg, NO_ERROR);
}

BOOL ServiceCommand::StopDependentServices(ServiceControl& sc, LPVOID lpvParam)
{
	BOOL ret = FALSE;
	DWORD i;
	DWORD dwBytesNeeded = 0;
	DWORD dwCount = 0;

	LPENUM_SERVICE_STATUS   lpNumServiceStatus = NULL;
	ENUM_SERVICE_STATUS     enumServiceStatus;

	// Pass a zero-length buffer to get the required buffer size.
	// �T�C�Y�O�̃o�b�t�@��n�����ƂŁA���ۂɕK�v�ȃT�C�Y���擾���܂��B
	if (::EnumDependentServices(sc.GetHandle(), SERVICE_ACTIVE, lpNumServiceStatus, dwBytesNeeded, &dwBytesNeeded, &dwCount))
	{
		// If the Enum call succeeds, then there are no dependent services, so do nothing.
		// �֐������������ꍇ�A�ˑ�����T�[�r�X�͂���܂���B����ĉ������܂���B
		PrintMessage(_T("�ˑ�����T�[�r�X�͂���܂���B"), NO_ERROR);
		ret = TRUE;
	}
	else
	{
		do
		{
			DWORD dwLastError = ::GetLastError();
			if (dwLastError != ERROR_MORE_DATA)
			{
				PrintMessage(_T("EnumDependentServices()"), dwLastError);
				break;
			}

			// Allocate a buffer for the dependencies.
			// �ˑ��֌W���擾���邽�߂Ƀo�b�t�@���������܂��B
			lpNumServiceStatus = (LPENUM_SERVICE_STATUS)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);
			if (!lpNumServiceStatus)
			{
				PrintMessage(_T("HeapAlloc()"));
				break;
			}

			// Enumerate the dependencies.
			// �ˑ�����T�[�r�X��񋓂��܂��B
			if (!::EnumDependentServices(sc.GetHandle(), SERVICE_ACTIVE, lpNumServiceStatus, dwBytesNeeded, &dwBytesNeeded, &dwCount))
			{
				PrintMessage(_T("EnumDependentServices()"));
				break;
			}

			for (i = 0; i < dwCount; i++)
			{
				enumServiceStatus = *(lpNumServiceStatus + i);
				ServiceControl dependentSC(sc.GetManager(), enumServiceStatus.lpServiceName);
				ret = dependentSC.Open(SERVICE_STOP | SERVICE_QUERY_STATUS);
				if (!ret)
				{
					PrintMessage(_T("OpenService()"));
					break;
				}

				if (!dependentSC.Stop())
				{
					PrintMessage(_T("ControlService()"));
					break;
				}

				QueryResponse response = dependentSC.QueryStatus(QueryResponse::StopPending);
				if (response == QueryResponse::Stopped)
				{
					PrintMessage(_T("�ˑ�����T�[�r�X���~���܂����B"), NO_ERROR);
				}
				else
				{
					PrintMessage(_T("�ˑ�����T�[�r�X�̒�~�Ɏ��s���܂����B"), NO_ERROR);
				}
				
				ret = dependentSC.Close();
				if (!ret)
				{
					PrintMessage(_T("CloseServiceHandle()"));
					break;
				}
			}

			ret = ::HeapFree(GetProcessHeap(), 0, lpNumServiceStatus);
			if (!ret)
			{
				PrintMessage(_T("HeapFree()"));
				break;
			}

		} while (0);
	}

	return ret;
}