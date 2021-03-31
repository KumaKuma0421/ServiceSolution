#include <Windows.h>
#include <tchar.h>
#include <locale.h>
#include <debugapi.h>
#include <stdio.h>
#include <exception>
#include "framework.h"
#include "Registry.h"

#define MAX_BUFF_SIZE 1024

#define SVC_SUCCESS_SYSTEM               ((DWORD)0x00000000L)
#define SVC_SUCCESS_APPLICATION          ((DWORD)0x00010000L)
#define SVC_SUCCESS_RUNTIME              ((DWORD)0x00020000L)
#define SVC_INFO_SYSTEM                  ((DWORD)0x40000000L)
#define SVC_INFO_RUNTIME                 ((DWORD)0x40020000L)
#define SVC_WARN_SYSTEM                  ((DWORD)0x80000000L)
#define SVC_WARN_RUNTIME                 ((DWORD)0x80020000L)
#define SVC_ERROR_SYSTEM                 ((DWORD)0xC0000000L)
#define SVC_ERROR_RUNTIME                ((DWORD)0xC0020000L)


VOID ReadRegValue();
BOOL GetErrorMessage(DWORD dwMessageID, LPTSTR lptszBuf, DWORD dwBufLen);
BOOL GetMessage(DWORD dwErrorCD, LPTSTR lptszBuf, DWORD dwBufLen, LPCTSTR* lpctszMessages);

int _tmain(int argc, TCHAR** argv)
{
	_tsetlocale(LC_ALL, _T("Japanese"));

	//ReadRegValue();

	int selection = 1;

	if (selection == 0)
	{
		TCHAR tszMsg[MAX_BUFF_SIZE];
		ZeroMemory(tszMsg, sizeof(tszMsg));

		for (DWORD dwNo = 0; dwNo < MAXDWORD; dwNo++)
		{
			BOOL ret = GetErrorMessage(dwNo, tszMsg, sizeof(tszMsg));
			if (ret)
				_tprintf(_T("%u %s"), dwNo, tszMsg);
			else
				_tprintf(_T("%u ----------\n"), dwNo);
		}
	}

	if(selection == 1)
	{
		TCHAR tszMsg[MAX_BUFF_SIZE];
		ZeroMemory(tszMsg, sizeof(tszMsg));
		DWORD adwKeys[] = {
			SVC_SUCCESS_SYSTEM,
			SVC_SUCCESS_APPLICATION,
			SVC_SUCCESS_RUNTIME,
			SVC_INFO_SYSTEM,
			SVC_INFO_RUNTIME,
			SVC_WARN_SYSTEM,
			SVC_WARN_RUNTIME,
			SVC_ERROR_SYSTEM,
			SVC_ERROR_RUNTIME,
		};

		LPCTSTR lpctszParams[] = {
			_T("TEST1"),
			_T("TEST2"),
			_T("TEST3"),
		};

		for (DWORD dwNo : adwKeys)
		{
			BOOL ret = GetMessage(dwNo, tszMsg, sizeof(tszMsg), lpctszParams);
			if (ret)
				_tprintf(_T("%u %s"), dwNo, tszMsg);
			else
				_tprintf(_T("%u ----------\n"), dwNo);
		}
	}
}

VOID ReadRegValue()
{
	HKEY hKeyRoot = HKEY_CURRENT_USER;
	LPCTSTR lpctszKeyName = _T("SOFTWARE\\Alternate\\Configuration");
	LPCTSTR lpctszEventMessageFile = _T("C:\\users\\user01\\source\\repos\\ConsoleSolution\\x64\\Debug\\sample.dll");

	BOOL ret;
	alt::Registry registry1;

	do
	{
		//
		// Open()、失敗ならCreate()
		//
		ret = registry1.Open(hKeyRoot, lpctszKeyName);
		if (!ret)
		{
			_tprintf(_T("Resistry::Open(%s) failed. Try to create.\n"), lpctszKeyName);
			
			ret = registry1.Create(hKeyRoot, lpctszKeyName);
			if (!ret)
			{
				_tprintf(_T("Registry::Create(%s) failed.\n"), lpctszKeyName);
				break;
			}
			else
			{
				_tprintf(_T("Registry::Create(%s) success.\n"), lpctszKeyName);
			}
		}
		else
		{
			_tprintf(_T("Registry::Open(%s) success.\n"), lpctszKeyName);
		}

		// -----------------------------------------
		// EventMessageFileの値を確認、なければ作成。
		// -----------------------------------------

		LPCTSTR lpctszEntry = _T("EventMessageFile");
		TCHAR tszValue[1024]{ _T("") };

		ret = registry1.QueryValue(lpctszEntry, alt::RegistryValueTypes::SZ, (LPBYTE)tszValue, 1024);
		if (!ret)
		{
			_tprintf(_T("Registry::QueryValue(%s) failed. Try to create.\n"), lpctszEntry);

			DWORD dwDataLen = static_cast<DWORD>(_tcslen(lpctszEventMessageFile) * sizeof(TCHAR));

			ret = registry1.SetValue(lpctszEntry, alt::RegistryValueTypes::SZ, (LPBYTE)lpctszEventMessageFile, dwDataLen);
			if (!ret)
			{
				_tprintf(_T("Registry::SetValue(%s) <- %s failed.\n"), lpctszEntry, lpctszEventMessageFile);
				break;
			}
			else
			{
				_tprintf(_T("Registry::SetValue(%s) <- %s success.\n"), lpctszEntry, lpctszEventMessageFile);
			}
		}
		else
		{
			_tprintf(_T("Registry::QueryValue(%s) = %s.\n"), lpctszEntry, tszValue);
		}

		// ---------------------------------------
		// TypesSupportedの値を確認、なければ作成。
		// ---------------------------------------

		lpctszEntry = _T("TypesSupported");
		DWORD dwTypesSupported = 7;

		ret = registry1.QueryValue(lpctszEntry, alt::RegistryValueTypes::DWORD, (LPBYTE)&dwTypesSupported, sizeof(dwTypesSupported));
		if (!ret)
		{
			_tprintf(_T("Registry::QueryValue(%s) failed. Try to create.\n"), lpctszEntry);

			ret = registry1.SetValue(lpctszEntry, alt::RegistryValueTypes::DWORD, (LPBYTE)&dwTypesSupported, sizeof(dwTypesSupported));
			if (!ret)
			{
				_tprintf(_T("Registry::SetValue(%s) <- %d failed.\n"), lpctszEntry, dwTypesSupported);
				break;
			}
			else
			{
				_tprintf(_T("Registry::SetValue(%s) <- %d success.\n"), lpctszEntry, dwTypesSupported);
			}
		}
		else
		{
			_tprintf(_T("Registry::QueryValue(%s) = %d.\n"), lpctszEntry, dwTypesSupported);
		}

		// --------------
		// キーのクエリー
		// --------------
		ret = registry1.Query();
		if (!ret)
		{
			_tprintf(_T("Registry::QueryKeys() failed.\n"));
			break;
		}
		else
		{
			_tprintf(_T("Registry::QueryKeys() success.\n"));
		}

		// -----------------------------------------
		// TypesSupportedの値を削除
		// -----------------------------------------

		lpctszEntry = _T("TypesSupported");

		ret = registry1.DeleteValue(lpctszEntry);
		if (!ret)
		{
			_tprintf(_T("Registry::DeleteValue(%s) failed.\n"), lpctszEntry);
			break;
		}
		else
		{
			_tprintf(_T("Registry::DeleteValue(%s) success.\n"), lpctszEntry);

			ret = registry1.QueryValue(lpctszEntry, alt::RegistryValueTypes::DWORD, (LPBYTE)&dwTypesSupported, sizeof(dwTypesSupported));
			if (!ret)
			{
				_tprintf(_T("Registry::QueryValue(%s) failed. Because of no value.\n"), lpctszEntry);
			}
			else
			{
				_tprintf(_T("Registry::QueryValue(%s) = %d success.\n"), lpctszEntry, dwTypesSupported);
			}
		}

		// -----------------------------------------
		// キーをフラッシュ
		// -----------------------------------------

		ret = registry1.Flush();
		if (!ret)
		{
			_tprintf(_T("Registry::Flush() failed.\n"));
			break;
		}
		else
		{
			_tprintf(_T("Registry::Flush() success.\n"));
		}

		// -----------------------------------------
		// サブキー1を作成
		// -----------------------------------------
		
		alt::Registry registry2;
		LPCTSTR lpctszSubKeyName = _T("TEST1");

		ret = registry2.Create(registry1.GetKey(), lpctszSubKeyName);
		if (!ret)
		{
			_tprintf(_T("Registry::Create(%s) failed.\n"), lpctszSubKeyName);
			break;
		}
		else
		{
			_tprintf(_T("Registry::Create(%s) success.\n"), lpctszSubKeyName);
		}

		// -----------------------------------------
		// サブキー2を作成
		// -----------------------------------------

		alt::Registry registry3;
		lpctszSubKeyName = _T("TEST2");

		ret = registry3.Create(registry1.GetKey(), lpctszSubKeyName);
		if (!ret)
		{
			_tprintf(_T("Registry::Create(%s) failed.\n"), lpctszSubKeyName);
			break;
		}
		else
		{
			_tprintf(_T("Registry::Create(%s) success.\n"), lpctszSubKeyName);
		}

		// -----------------------------------------
		// サブキー1に値を設定
		// -----------------------------------------

		lpctszEntry = _T("SampleString1");
		ZeroMemory(tszValue, sizeof(tszValue));

		ret = registry2.QueryValue(lpctszEntry, alt::RegistryValueTypes::SZ, (LPBYTE)tszValue, 1024);
		if (!ret)
		{
			_tprintf(_T("Registry::QueryValue(%s) failed. Try to create.\n"), lpctszEntry);

			DWORD dwDataLen = static_cast<DWORD>(_tcslen(lpctszEventMessageFile) * sizeof(TCHAR));

			ret = registry2.SetValue(lpctszEntry, alt::RegistryValueTypes::SZ, (LPBYTE)lpctszEventMessageFile, dwDataLen);
			if (!ret)
			{
				_tprintf(_T("Registry::SetValue(%s) <- %s failed.\n"), lpctszEntry, lpctszEventMessageFile);
				break;
			}
			else
			{
				_tprintf(_T("Registry::SetValue(%s) <- %s success.\n"), lpctszEntry, lpctszEventMessageFile);
			}
		}
		else
		{
			_tprintf(_T("Registry::QueryValue(%s) = %s.\n"), lpctszEntry, tszValue);
		}

		// -----------------------------------------
		// サブキー2を削除
		// -----------------------------------------
		
		ret = registry1.DeleteKey(lpctszSubKeyName);
		if (!ret)
		{
			_tprintf(_T("Registry::DeleteKey(%s) failed.\n"), lpctszSubKeyName);
			break;
		}
		else
		{
			_tprintf(_T("Registry::DeleteKey(%s) success.\n"), lpctszSubKeyName);
		}

		// -----------------------------------------
		// サブキー2をクローズ
		// -----------------------------------------

		ret = registry2.Close();
		if (!ret)
		{
			_tprintf(_T("Registry::Close() failed.\n"));
			break;
		}
		else
		{
			_tprintf(_T("Registry::Close() success.\n"));
		}

		// -----------------------------------------
		// キーをクローズ
		// -----------------------------------------

		ret = registry1.Close();
		if (!ret)
		{
			_tprintf(_T("Registry::Close() failed.\n"));
			break;
		}
		else
		{
			_tprintf(_T("Registry::Close() success.\n"));
		}

		// -----------------------------------------
		// 完全消去
		// -----------------------------------------

		lpctszKeyName = _T("SOFTWARE");
		alt::Registry registry;

		ret = registry.Open(hKeyRoot, lpctszKeyName);
		if (!ret)
		{
			_tprintf(_T("Registry::Open(%s) failed.\n"), lpctszKeyName);
			return;
		}
		else
		{
			_tprintf(_T("Registry::Open(%s) success.\n"), lpctszKeyName);
		}

		lpctszSubKeyName = _T("Alternate");

		ret = registry.DeleteTree(lpctszSubKeyName);
		if (!ret)
		{
			_tprintf(_T("Registry::DeleteTree(%s) failed.\n"), lpctszSubKeyName);
		}
		else
		{
			_tprintf(_T("Registry::DeleteTree(%s) success.\n"), lpctszSubKeyName);
		}


	} while (0);
}

BOOL GetErrorMessage(DWORD dwErrorCD, LPTSTR lptszBuf, DWORD dwBufLen)
{
	DWORD dwRet = ::FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		dwErrorCD,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lptszBuf,
		dwBufLen,
		nullptr);

	return dwRet > 0 ? TRUE : FALSE;
}

BOOL GetMessage(DWORD dwErrorCD, LPTSTR lptszBuf, DWORD dwBufLen, LPCTSTR* lpctszMessages)
{
	LPCTSTR lpctszLibFileName = _T("C:\\Users\\User01\\source\\repos\\ConsoleSolution\\x64\\Debug\\sample.dll");
	HANDLE hFile = nullptr;
	DWORD dwFlags = DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE | FORMAT_MESSAGE_ARGUMENT_ARRAY;

	HMODULE hModule = ::LoadLibraryEx(lpctszLibFileName, hFile, dwFlags);

	DWORD dwRet = ::FormatMessage(
		FORMAT_MESSAGE_FROM_HMODULE,
		hModule,
		dwErrorCD,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lptszBuf,
		dwBufLen,
		(va_list*)&lpctszMessages);

	return dwRet > 0 ? TRUE : FALSE;
}
