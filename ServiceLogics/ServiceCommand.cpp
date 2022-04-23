//! @file   ServiceCommand.cpp
//! @brief  サービスに対する操作を集約したクラス
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#include "pch.h"
#include "ServiceCommand.h"
#include "Registry.h"

ServiceCommand::ServiceCommand (ServiceInfo& si)
	:_si (si)
{

}

BOOL ServiceCommand::Command (LPCTSTR lpctszCommand, LPCTSTR lpctszOption)
{
	BOOL ret = FALSE;

	if (_tcscmp (lpctszCommand, COMMAND_INSTALL) == 0)
	{
		ret = Install ();
	}
	else if (_tcscmp (lpctszCommand, COMMAND_STATUS) == 0)
	{
		ret = Status ();
	}
	else if (_tcscmp (lpctszCommand, COMMAND_REMOVE) == 0)
	{
		ret = Remove ();
	}
	else if (_tcscmp (lpctszCommand, COMMAND_ENABLE) == 0)
	{
		ret = Enable (TRUE);
	}
	else if (_tcscmp (lpctszCommand, COMMAND_ONDEMAND) == 0)
	{
		ret = Enable (FALSE);
	}
	else if (_tcscmp (lpctszCommand, COMMAND_DISABLE) == 0)
	{
		ret = Disable ();
	}
	else if (_tcscmp (lpctszCommand, COMMAND_CHANGE_DESCRIPTION) == 0)
	{
		ret = ChangeConfig2Description ();
	}
	else if (_tcscmp (lpctszCommand, COMMAND_CHANGE_DELAYED_AUTO_START) == 0)
	{
		ret = ChangeConfig2DelayedAutoStart ();
	}
	else if (_tcscmp (lpctszCommand, COMMAND_START) == 0)
	{
		ret = Start ();
	}
	else if (_tcscmp (lpctszCommand, COMMAND_STOP) == 0)
	{
		ret = Stop ();
	}
	else
	{
		PrintMessage (_T ("不明なコマンドです。"), NO_ERROR, lpctszCommand);
	}

	return ret;
}

BOOL ServiceCommand::Install ()
{
	auto InstallAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret = sc.ChangeConfig2Description ();
		if (!ret)
		{
			PrintMessage (_T ("ChangeServiceConfig2()"));
		}

		return ret;
	};

	//
	// @note 「アプリケーションとサービスログ」の直下に独自の
	// 	     イベントビューアーログを生成した後は、システムの再起動が
	// 	   　必要です。
	//
	// HKEY_LOCAL_MACHINE
	// └SYSTEM
	//   └CurrentControlSet
	//     └Services
	//       └EventLog
	//         └ServiceApplication
	//           ・MaxSize:REG_DWORD(0x1400000)
	//           ・Retention:REG_DWORD(0x0)
	//           ・File:REG_EXPAND_SZ:"%SystemRoot%\System32\Winevt\Logs\ServiceApplication.evtx"
	//

	BOOL ret;
	HKEY hKeyRoot = HKEY_LOCAL_MACHINE;
	RegistryValueTypes eType;
	BOOL bFirstSetting = FALSE;

	do
	{
		Registry registry1;

		ret = registry1.Open (hKeyRoot, LOG_ROOT);
		if (!ret)
		{
			ret = registry1.Create (hKeyRoot, LOG_ROOT);
			if (!ret)
			{
				PrintMessage (ERROR_REGISTRY_REGISTER, GetLastError (), LOG_ROOT);
				break;
			}

			bFirstSetting = TRUE;
		}

		DWORD dwMaxSize = 0;

		ret = registry1.QueryValue (ENTRY_MAX_SIZE, eType, (LPBYTE)&dwMaxSize, sizeof (dwMaxSize));
		if (!ret || dwMaxSize != LOG_MAX_SIZE)
		{
			dwMaxSize = LOG_MAX_SIZE;
			ret = registry1.SetValue (ENTRY_MAX_SIZE, RegistryValueTypes::DWORD, (LPBYTE)&dwMaxSize, sizeof (dwMaxSize));
			if (!ret)
			{
				PrintMessage (ERROR_REGISTRY_REGISTER, GetLastError (), ENTRY_MAX_SIZE);
				break;
			}

			bFirstSetting = TRUE;
		}

		DWORD dwRetention = 0xFFFFFFFF;

		ret = registry1.QueryValue (ENTRY_RETENTION, eType, (LPBYTE)&dwRetention, sizeof (dwRetention));
		if (!ret || dwRetention != LOG_RETENTION)
		{
			dwRetention = LOG_RETENTION;
			ret = registry1.SetValue (ENTRY_RETENTION, RegistryValueTypes::DWORD, (LPBYTE)&dwRetention, sizeof (dwRetention));
			if (!ret)
			{
				PrintMessage (ERROR_REGISTRY_REGISTER, GetLastError (), ENTRY_RETENTION);
				break;
			}

			bFirstSetting = TRUE;
		}

		TCHAR tszFile[MAX_PATH] = { 0 };

		ret = registry1.QueryValue (ENTRY_FILE, eType, (LPBYTE)tszFile, MAX_PATH * sizeof (TCHAR));
		if (!ret || _tcscmp (tszFile, LOG_FILE) != 0)
		{
			ret = registry1.SetValue (ENTRY_FILE, RegistryValueTypes::EXPAND_SZ, (LPBYTE)LOG_FILE, (DWORD)(_tcslen (LOG_FILE) * sizeof (TCHAR)));
			if (!ret)
			{
				PrintMessage (ERROR_REGISTRY_REGISTER, GetLastError (), ENTRY_FILE);
				break;
			}

			bFirstSetting = TRUE;
		}
	} while (0);

	if (bFirstSetting)
	{
		PrintMessage (_T ("イベントビューアーのログファイル設定を行いましたので、システムの再起動を行ってください。"));
		return TRUE;
	}

	//
	// HKEY_LOCAL_MACHINE
	// └SYSTEM
	//   └CurrentControlSet
	//     └Services
	//       └EventLog
	//         └ServiceApplication
	//           └<サービス名>
	//             ・CategoryCount:REG_DWORD(0x4)
	//             ・CategoryMessageFile:REG_SZ:"C:\Users\User01\source\repos\ServiceSolution\x64\Debug\ServiceMessage.dll"
	//             ・EventMessageFile:REG_SZ:"C:\Users\User01\source\repos\ServiceSolution\x64\Debug\ServiceMessage.dll"
	//             ・TypesSupported:REG_DWORD(0x7)
	//

	do
	{
		Registry registry2;
		TCHAR tszRootKey[MAX_PATH];

		wsprintf (tszRootKey, _T ("%s\\%s"), EVENT_ROOT, _si.GetName ());

		ret = registry2.Open (hKeyRoot, tszRootKey);
		if (!ret)
		{
			ret = registry2.Create (hKeyRoot, tszRootKey);
			if (!ret)
			{
				PrintMessage (ERROR_REGISTRY_REGISTER, GetLastError (), tszRootKey);
				break;
			}
		}

		DWORD dwCategoryCount = 0;

		ret = registry2.QueryValue (ENTRY_CATEGORY_COUNT, eType, (LPBYTE)&dwCategoryCount, sizeof (dwCategoryCount));
		if (!ret || dwCategoryCount != EVENT_CATEGORY_COUNT)
		{
			dwCategoryCount = EVENT_CATEGORY_COUNT;
			ret = registry2.SetValue (ENTRY_CATEGORY_COUNT, RegistryValueTypes::DWORD, (LPBYTE)&dwCategoryCount, sizeof (dwCategoryCount));
			if (!ret)
			{
				PrintMessage (ERROR_REGISTRY_REGISTER, GetLastError (), ENTRY_CATEGORY_COUNT);
				break;
			}
		}

		DWORD dwTypesSupported = 0;

		ret = registry2.QueryValue (ENTRY_TYPES_SUPPORTED, eType, (LPBYTE)&dwTypesSupported, sizeof (dwTypesSupported));
		if (!ret || dwTypesSupported != EVENT_TYPES_SUPPORTED)
		{
			dwTypesSupported = EVENT_TYPES_SUPPORTED;
			ret = registry2.SetValue (ENTRY_TYPES_SUPPORTED, RegistryValueTypes::DWORD, (LPBYTE)&dwTypesSupported, sizeof (dwTypesSupported));
			if (!ret)
			{
				PrintMessage (ERROR_REGISTRY_REGISTER, GetLastError (), ENTRY_TYPES_SUPPORTED);
				break;
			}
		}

		TCHAR tszRequiredFilePath[MAX_PATH] = { 0 };
		TCHAR tszCategoryMessageFile[MAX_PATH] = { 0 };
		TCHAR tszEventMessageFile[MAX_PATH] = { 0 };

		wsprintf (tszRequiredFilePath, _T ("%s\\%s"), _si.GetWorkDirectory (), EVENT_CATEGORY_MESSAGE_FILE);

		ret = registry2.QueryValue (ENTRY_CATEGORY_MESSAGE_FILE, eType, (LPBYTE)tszCategoryMessageFile, (DWORD)(sizeof (tszCategoryMessageFile)));
		if (!ret || _tcscmp (tszCategoryMessageFile, tszRequiredFilePath) != 0)
		{
			ret = registry2.SetValue (ENTRY_CATEGORY_MESSAGE_FILE, RegistryValueTypes::SZ, (LPBYTE)tszRequiredFilePath, (DWORD)(_tcslen (tszRequiredFilePath) * sizeof (TCHAR)));
			if (!ret)
			{
				PrintMessage (ERROR_REGISTRY_REGISTER, GetLastError (), ENTRY_CATEGORY_MESSAGE_FILE);
				break;
			}
		}

		wsprintf (tszRequiredFilePath, _T ("%s\\%s"), _si.GetWorkDirectory (), EVENT_EVENT_MESSAGE_FILE);

		ret = registry2.QueryValue (ENTRY_EVENT_MESSAGE_FILE, eType, (LPBYTE)tszEventMessageFile, (DWORD (sizeof (tszEventMessageFile))));
		if (!ret || _tcscmp (tszEventMessageFile, tszRequiredFilePath) != 0)
		{
			ret = registry2.SetValue (ENTRY_EVENT_MESSAGE_FILE, RegistryValueTypes::SZ, (LPBYTE)tszRequiredFilePath, (DWORD)(_tcslen (tszRequiredFilePath) * sizeof (TCHAR)));
			if (!ret)
			{
				PrintMessage (ERROR_REGISTRY_REGISTER, GetLastError (), ENTRY_EVENT_MESSAGE_FILE);
				break;
			}
		}

		ret = TemplateAction (
			0,
			InstallAction,
			_T ("サービスをインストールしました。"));

	} while (0);

	return ret;
}

BOOL ServiceCommand::Status ()
{
	auto StatusAction = [](ServiceControl& sc, void* lpvParam)
	{
		SERVICE_STATUS_PROCESS status = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		BOOL ret = sc.QueryStatusEx (status);
		if (!ret)
		{
			PrintMessage (_T ("QueryServiceStatusEx()"));
		}
		else
		{
			_tprintf (_T ("ServiceType:\n"));
			if (status.dwServiceType & SERVICE_KERNEL_DRIVER)
				_tprintf (_T ("  KERNEL_DRIVER\n"));
			else if (status.dwServiceType & SERVICE_FILE_SYSTEM_DRIVER)
				_tprintf (_T ("  FILE_SYSTEM_DRIVER\n"));
			else if (status.dwServiceType & SERVICE_WIN32_OWN_PROCESS)
				_tprintf (_T ("  WIN32_OWN_PROCESS\n"));
			else if (status.dwServiceType & SERVICE_WIN32_SHARE_PROCESS)
				_tprintf (_T ("  WIN32_SHARE_PROCESS\n"));
			else
				_tprintf (_T ("  Unknown\n"));
			if (status.dwServiceType & SERVICE_INTERACTIVE_PROCESS)
				_tprintf (_T ("  SERVICE_INTERACTIVE_PROCESS\n")); // can be combined.

			_tprintf (_T ("CurrentState:\n"));
			switch (status.dwCurrentState)
			{
				case SERVICE_STOPPED:
					_tprintf (_T ("  STOPPED\n"));
					break;

				case SERVICE_START_PENDING:
					_tprintf (_T ("  START_PENDING\n"));
					break;

				case SERVICE_STOP_PENDING:
					_tprintf (_T ("  STOP_PENDING\n"));
					break;

				case SERVICE_RUNNING:
					_tprintf (_T ("  RUNNING\n"));
					break;

				case SERVICE_CONTINUE_PENDING:
					_tprintf (_T ("  CONTINUE_PENDING\n"));
					break;

				case SERVICE_PAUSE_PENDING:
					_tprintf (_T ("  PAUSE_PENDING\n"));
					break;

				case SERVICE_PAUSED:
					_tprintf (_T ("  PAUSED\n"));
					break;

				default:
					_tprintf (_T ("  unknown\n"));
					break;
			}

			if (status.dwControlsAccepted > 0)
			{
				_tprintf (_T ("ControlsAccepted:\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_STOP)
					_tprintf (_T ("  STOP\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_PAUSE_CONTINUE)
					_tprintf (_T ("  PAUSE_CONTINUE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_SHUTDOWN)
					_tprintf (_T ("  SHUTDOWN\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_PARAMCHANGE)
					_tprintf (_T ("  PARAMCHANGE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_NETBINDCHANGE)
					_tprintf (_T ("  NETBINDCHANGE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_HARDWAREPROFILECHANGE)
					_tprintf (_T ("  HARDWAREPROFILECHANGE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_POWEREVENT)
					_tprintf (_T ("  POWEREVENT\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_SESSIONCHANGE)
					_tprintf (_T ("  SESSIONCHANGE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_PRESHUTDOWN)
					_tprintf (_T ("  PRESHUTDOWN\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_TIMECHANGE)
					_tprintf (_T ("  TIMECHANGE\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_TRIGGEREVENT)
					_tprintf (_T ("  TRIGGEREVENT\n"));
				if (status.dwControlsAccepted & SERVICE_ACCEPT_USER_LOGOFF)
					_tprintf (_T ("  USER_LOGOFF\n"));
			}

			_tprintf (_T ("Win32ExitCode=%u\n"), status.dwWin32ExitCode);
			_tprintf (_T ("ServiceSpecificExitCode=%u\n"), status.dwServiceSpecificExitCode);
			_tprintf (_T ("CheckPoint=%u\n"), status.dwCheckPoint);
			_tprintf (_T ("WaitHint=%u\n"), status.dwWaitHint);
			_tprintf (_T ("ProcessId=%u\n"), status.dwProcessId);
			_tprintf (_T ("ServiceFlags=%u\n"), status.dwServiceFlags);
		}

		LPQUERY_SERVICE_CONFIG lpQueryServiceConfig = nullptr;
		ret = sc.QueryConfig (lpQueryServiceConfig);
		if (!ret || lpQueryServiceConfig == nullptr)
		{
			PrintMessage (_T ("QueryServiceConfig()"));
		}
		else
		{
			_tprintf (_T ("dwServiceType:%d\n"), lpQueryServiceConfig->dwServiceType);
			_tprintf (_T ("dwStartType:%d\n"), lpQueryServiceConfig->dwStartType);
			_tprintf (_T ("dwErrorControl:%d\n"), lpQueryServiceConfig->dwErrorControl);
			_tprintf (_T ("lpBinaryPathName:%s\n"), lpQueryServiceConfig->lpBinaryPathName);
			_tprintf (_T ("lpLoadOrderGroup:%s\n"), lpQueryServiceConfig->lpLoadOrderGroup);
			_tprintf (_T ("dwTagId:%d\n"), lpQueryServiceConfig->dwTagId);
			_tprintf (_T ("lpDependencies:%s\n"), lpQueryServiceConfig->lpDependencies);
			_tprintf (_T ("lpServiceStartName:%s\n"), lpQueryServiceConfig->lpServiceStartName);
			_tprintf (_T ("lpDisplayName:%s\n"), lpQueryServiceConfig->lpDisplayName);

			LocalFree (lpQueryServiceConfig);
		}

		return ret;
	};

	return TemplateAction (
		SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG,
		StatusAction,
		nullptr);
}

BOOL ServiceCommand::Remove ()
{
	auto RemoveAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret = sc.Delete ();
		if (!ret)
		{
			PrintMessage (_T ("DeleteService()"));
		}

		return ret;
	};

	return TemplateAction (
		DELETE,
		RemoveAction,
		_T ("サービスをアンインストールしました。"));
}

BOOL ServiceCommand::Enable (BOOL bAuto)
{
	auto EnableAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret;
		BOOL bAuto = *((LPBOOL)lpvParam);

		do
		{
			ret = sc.ChangeConfig (bAuto == TRUE ? BootSettings::Auto : BootSettings::OnDemand);
			if (!ret)
			{
				PrintMessage (_T ("ChangeServiceConfig()"));
				break;
			}

			ret = sc.ChangeConfig2DelayedAutoStart (FALSE);
			if (!ret)
			{
				PrintMessage (_T ("ChangeServiceConfig2()"));
				break;
			}
		} while (0);

		return ret;
	};

	LPCTSTR lpctszMsg = bAuto ? _T ("サービスを自動起動にしました。") : _T ("サービスを手動起動にしました。");

	return TemplateAction (
		SERVICE_CHANGE_CONFIG,
		EnableAction,
		lpctszMsg,
		&bAuto);
}

BOOL ServiceCommand::Disable ()
{
	auto DisableAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret;

		do
		{
			ret = sc.ChangeConfig (BootSettings::Disable);
			if (!ret)
			{
				PrintMessage (_T ("ChangeServiceConfig()"));
				break;
			}

			ret = sc.ChangeConfig2DelayedAutoStart (FALSE);
			if (!ret)
			{
				PrintMessage (_T ("ChangeServiceConfig2()"));
				break;
			}
		} while (0);

		return ret;
	};

	return TemplateAction (
		SERVICE_CHANGE_CONFIG,
		DisableAction,
		_T ("サービスを無効にしました。"));
}

BOOL ServiceCommand::ChangeConfig2Description ()
{
	auto DescriptionAction = [](ServiceControl& sc, void* lpvParam)
	{
		LPCTSTR lpctszDescription = (LPCTSTR)lpvParam;

		BOOL ret = sc.ChangeConfig2Description ();
		if (!ret)
		{
			PrintMessage (_T ("ChangeServiceConfig2()"));
		}

		return ret;
	};

	return TemplateAction (
		SERVICE_CHANGE_CONFIG,
		DescriptionAction,
		_T ("サービスの説明を変更しました。"));
}

BOOL ServiceCommand::ChangeConfig2DelayedAutoStart ()
{
	auto DelayedAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret = sc.ChangeConfig2DelayedAutoStart ();
		if (!ret)
		{
			PrintMessage (_T ("ChangeServiceConfig2()"));
		}

		return ret;
	};

	return TemplateAction (
		SERVICE_CHANGE_CONFIG,
		DelayedAction,
		_T ("サービスを自動起動(遅延)にしました。"));
}

BOOL ServiceCommand::Start ()
{
	auto StartAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret = FALSE;
		LPCTSTR lpctszMsg = nullptr;
		ServiceCommand* me = (ServiceCommand*)lpvParam;

		QueryResponse response = sc.QueryStatus (QueryResponse::StopPending);

		do
		{
			me->ShowCurrentStatus (response);

			if (response == QueryResponse::Stopped)
			{
				ret = sc.Start ();
				if (!ret)
				{
					PrintMessage (_T ("StartService()"));
					break;
				}

				response = sc.QueryStatus (QueryResponse::StartPending);
				if (response == QueryResponse::Running)
				{
					PrintMessage (_T ("サービスを開始しました。"), NO_ERROR);
				}
				else
				{
					PrintMessage (_T ("サービス開始に失敗しました。"), NO_ERROR);
				}
			}
		} while (0);

		return ret;
	};

	return TemplateAction (
		SERVICE_ALL_ACCESS,
		StartAction,
		nullptr,
		this);
}

BOOL ServiceCommand::Stop ()
{
	auto StopAction = [](ServiceControl& sc, void* lpvParam)
	{
		BOOL ret = FALSE;
		ServiceCommand* me = (ServiceCommand*)lpvParam;

		QueryResponse response = sc.QueryStatus (QueryResponse::StopPending);

		do
		{
			me->ShowCurrentStatus (response);

			if (response == QueryResponse::StartPending ||
				response == QueryResponse::Running ||
				response == QueryResponse::ContinuePending)
			{
				me->StopDependentServices (sc, lpvParam);

				ret = sc.Stop ();
				if (!ret)
				{
					PrintMessage (_T ("StopService()"));
					break;
				}

				response = sc.QueryStatus (QueryResponse::StopPending);
				if (response == QueryResponse::Stopped)
				{
					PrintMessage (_T ("サービスを停止しました。"), NO_ERROR);
				}
				else
				{
					PrintMessage (_T ("サービス停止に失敗しました。"), NO_ERROR);
				}
			}
		} while (0);

		return ret;
	};

	return TemplateAction (
		SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS,
		StopAction,
		nullptr,
		this);
}

// ------------
// PRIVATE ZONE
// ------------

BOOL ServiceCommand::TemplateAction (
	DWORD dwOpenParam,
	TemplateMethod method,
	LPCTSTR lpctszLastMessage,
	LPVOID lpvParam)
{
	BOOL ret = FALSE;
	ServiceControlManager scm;
	ServiceControl sc (scm, _si);

	do
	{
		ret = scm.Open ();
		if (!ret)
		{
			PrintMessage (_T ("OpenSCManager()"));
			break;
		}

		if (dwOpenParam == 0)
		{
			ret = sc.Create ();
			if (!ret)
			{
				PrintMessage (_T ("CreateService()"));
				break;
			}
		}
		else
		{
			ret = sc.Open (dwOpenParam);
			if (!ret)
			{
				PrintMessage (_T ("OpenService()"));
				break;
			}
		}

		ret = method (sc, lpvParam);
		if (!ret) break;

		ret = sc.Close ();
		if (!ret)
		{
			PrintMessage (_T ("CloseServiceHandle()"));
			break;
		}

		ret = scm.Close ();
		if (!ret)
		{
			PrintMessage (_T ("CloseServiceHandle()"));
			break;
		}

		if (lpctszLastMessage)
			PrintMessage (lpctszLastMessage);

		ret = TRUE;

	} while (0);

	return ret;
}

VOID ServiceCommand::ShowCurrentStatus (QueryResponse response)
{
	LPCTSTR lpctszMsg = nullptr;

	switch (response)
	{
		case QueryResponse::Error:
			lpctszMsg = _T ("サービスの起動確認中にエラーが発生しました。");
			break;

		case QueryResponse::Timeout:
			lpctszMsg = _T ("サービスの起動確認中にタイムアウトが発生しました。");
			break;

		case QueryResponse::Stopped:
			lpctszMsg = _T ("サービスは停止しています。");
			break;

		case QueryResponse::StartPending:
		case QueryResponse::Running:
		case QueryResponse::ContinuePending:
			lpctszMsg = _T ("サービスは起動しています。");
			break;

		case QueryResponse::PausePending:
		case QueryResponse::Paused:
			lpctszMsg = _T ("サービスは中断状態です。");
			break;

		case QueryResponse::StopPending:
		default:
			lpctszMsg = _T ("ランタイムエラーが発生しました。");
			break;
	}

	PrintMessage (lpctszMsg, NO_ERROR);
}

BOOL ServiceCommand::StopDependentServices (ServiceControl& sc, LPVOID lpvParam)
{
	BOOL ret = FALSE;
	DWORD i;
	DWORD dwBytesNeeded = 0;
	DWORD dwCount = 0;

	LPENUM_SERVICE_STATUS   lpNumServiceStatus = NULL;
	ENUM_SERVICE_STATUS     enumServiceStatus;

	// Pass a zero-length buffer to get the required buffer size.
	// サイズ０のバッファを渡すことで、実際に必要なサイズを取得します。
	if (::EnumDependentServices (sc.GetHandle (), SERVICE_ACTIVE, lpNumServiceStatus, dwBytesNeeded, &dwBytesNeeded, &dwCount))
	{
		// If the Enum call succeeds, then there are no dependent services, so do nothing.
		// 関数が成功した場合、依存するサービスはありません。よって何もしません。
		PrintMessage (_T ("依存するサービスはありません。"), NO_ERROR);
		ret = TRUE;
	}
	else
	{
		do
		{
			DWORD dwLastError = ::GetLastError ();
			if (dwLastError != ERROR_MORE_DATA)
			{
				PrintMessage (_T ("EnumDependentServices()"), dwLastError);
				break;
			}

			// Allocate a buffer for the dependencies.
			// 依存関係を取得するためにバッファを準備します。
			lpNumServiceStatus = (LPENUM_SERVICE_STATUS)::HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, dwBytesNeeded);
			if (!lpNumServiceStatus)
			{
				PrintMessage (_T ("HeapAlloc()"));
				break;
			}

			// Enumerate the dependencies.
			// 依存するサービスを列挙します。
			if (!::EnumDependentServices (sc.GetHandle (), SERVICE_ACTIVE, lpNumServiceStatus, dwBytesNeeded, &dwBytesNeeded, &dwCount))
			{
				PrintMessage (_T ("EnumDependentServices()"));
				break;
			}

			for (i = 0; i < dwCount; i++)
			{
				enumServiceStatus = *(lpNumServiceStatus + i);
				ServiceInfo si (enumServiceStatus.lpServiceName); // TODO:暫定的…
				ServiceControl dependentSC (sc.GetManager (), si);
				ret = dependentSC.Open (SERVICE_STOP | SERVICE_QUERY_STATUS);
				if (!ret)
				{
					PrintMessage (_T ("OpenService()"));
					break;
				}

				if (!dependentSC.Stop ())
				{
					PrintMessage (_T ("ControlService()"));
					break;
				}

				QueryResponse response = dependentSC.QueryStatus (QueryResponse::StopPending);
				if (response == QueryResponse::Stopped)
				{
					PrintMessage (_T ("依存するサービスを停止しました。"), NO_ERROR);
				}
				else
				{
					PrintMessage (_T ("依存するサービスの停止に失敗しました。"), NO_ERROR);
				}

				ret = dependentSC.Close ();
				if (!ret)
				{
					PrintMessage (_T ("CloseServiceHandle()"));
					break;
				}
			}

			ret = ::HeapFree (GetProcessHeap (), 0, lpNumServiceStatus);
			if (!ret)
			{
				PrintMessage (_T ("HeapFree()"));
				break;
			}

		} while (0);
	}

	return ret;
}