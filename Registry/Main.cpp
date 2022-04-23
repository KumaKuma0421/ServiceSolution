//
// Main.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"
#include "Registry.h"

int _tmain (int argc, TCHAR** argv)
{
	HKEY hKeyRoot = HKEY_CURRENT_USER;
	LPCTSTR lpctszKeyName = _T ("SOFTWARE\\Alternate\\Configuration");

	BOOL ret;

	do
	{
		Registry registry;
		RegistryValueTypes eType;

		//
		// Open()、失敗ならCreate()
		//
		ret = registry.Open (hKeyRoot, lpctszKeyName);
		if (!ret)
		{
			_tprintf (_T ("Resistry::Open(%s) failed. Try to create.\n"), lpctszKeyName);

			ret = registry.Create (hKeyRoot, lpctszKeyName);
			if (!ret)
			{
				_tprintf (_T ("Registry::Create(%s) failed.\n"), lpctszKeyName);
				break;
			}
			else
			{
				_tprintf (_T ("Registry::Create(%s) success.\n"), lpctszKeyName);
			}
		}
		else
		{
			_tprintf (_T ("Registry::Open(%s) success.\n"), lpctszKeyName);
		}

		// -----------------------------------------
		// EventMessageFileの値を確認、なければ作成。
		// -----------------------------------------

		LPCTSTR lpctszEntry = _T ("EventMessageFile");
		LPCTSTR lpctszEventMessageFile = _T ("C:\\users\\user01\\source\\repos\\ConsoleSolution\\x64\\Debug\\sample.dll");
		TCHAR tszValue[1024]{ _T ("") };

		ret = registry.QueryValue (lpctszEntry, eType, (LPBYTE)tszValue, 1024);
		if (!ret)
		{
			_tprintf (_T ("Registry::QueryValue(%s) failed. Try to create.\n"), lpctszEntry);

			DWORD dwDataLen = static_cast<DWORD>(_tcslen (lpctszEventMessageFile) * sizeof (TCHAR));

			ret = registry.SetValue (lpctszEntry, RegistryValueTypes::SZ, (LPBYTE)lpctszEventMessageFile, dwDataLen);
			if (!ret)
			{
				_tprintf (_T ("Registry::SetValue(%s) <- %s failed.\n"), lpctszEntry, lpctszEventMessageFile);
				break;
			}
			else
			{
				ret = registry.QueryValue (lpctszEntry, eType, (LPBYTE)tszValue, 1024);
				if (!ret)
				{
					_tprintf (_T ("Registry::QueryValue(%s) failed.\n"), lpctszEntry);
					break;
				}
				else
				{
					_tprintf (_T ("Registry::QueryValue[%d](%s) = %s.\n"), eType, lpctszEntry, tszValue);
				}
			}
		}
		else
		{
			_tprintf (_T ("Registry::QueryValue[%d](%s) = %s.\n"), eType, lpctszEntry, tszValue);
		}

		// ---------------------------------------
		// TypesSupportedの値を確認、なければ作成。
		// ---------------------------------------

		lpctszEntry = _T ("TypesSupported");
		DWORD dwTypesSupported = 7;

		ret = registry.QueryValue (lpctszEntry, eType, (LPBYTE)&dwTypesSupported, sizeof (dwTypesSupported));
		if (!ret)
		{
			_tprintf (_T ("Registry::QueryValue(%s) failed. Try to create.\n"), lpctszEntry);

			ret = registry.SetValue (lpctszEntry, RegistryValueTypes::DWORD, (LPBYTE)&dwTypesSupported, sizeof (dwTypesSupported));
			if (!ret)
			{
				_tprintf (_T ("Registry::SetValue(%s) <- %d failed.\n"), lpctszEntry, dwTypesSupported);
				break;
			}
			else
			{
				ret = registry.QueryValue (lpctszEntry, eType, (LPBYTE)&dwTypesSupported, sizeof (dwTypesSupported));
				if (!ret)
				{
					_tprintf (_T ("Registry::QueryValue(%s) failed.\n"), lpctszEntry);
					break;
				}
				else
				{
					_tprintf (_T ("Registry::QueryValue[%d](%s) = %d.\n"), eType, lpctszEntry, dwTypesSupported);
				}
			}
		}
		else
		{
			_tprintf (_T ("Registry::QueryValue[%d](%s) = %d.\n"), eType, lpctszEntry, dwTypesSupported);
		}

		// --------------
		// キーのクエリー
		// --------------
		ret = registry.Query ();
		if (!ret)
		{
			_tprintf (_T ("Registry::QueryKeys() failed.\n"));
			break;
		}
		else
		{
			_tprintf (_T ("Registry::QueryKeys() success.\n"));
		}

		// -----------------------------------------
		// TypesSupportedの値を削除
		// -----------------------------------------

		lpctszEntry = _T ("TypesSupported");

		ret = registry.DeleteValue (lpctszEntry);
		if (!ret)
		{
			_tprintf (_T ("Registry::DeleteValue(%s) failed.\n"), lpctszEntry);
			break;
		}
		else
		{
			_tprintf (_T ("Registry::DeleteValue(%s) success.\n"), lpctszEntry);

			ret = registry.QueryValue (lpctszEntry, eType, (LPBYTE)&dwTypesSupported, sizeof (dwTypesSupported));
			if (!ret)
			{
				_tprintf (_T ("Registry::QueryValue(%s) failed. Because of no value.\n"), lpctszEntry);
			}
			else
			{
				_tprintf (_T ("Registry::QueryValue(%s) = %d success.\n"), lpctszEntry, dwTypesSupported);
			}
		}

		// -----------------------------------------
		// キーをフラッシュ
		// -----------------------------------------

		ret = registry.Flush ();
		if (!ret)
		{
			_tprintf (_T ("Registry::Flush() failed.\n"));
			break;
		}
		else
		{
			_tprintf (_T ("Registry::Flush() success.\n"));
		}

		// -----------------------------------------
		// サブキー1を作成
		// -----------------------------------------

		Registry registry1;
		LPCTSTR lpctszSubKeyName = _T ("TEST1");

		ret = registry1.Create (registry.GetKey (), lpctszSubKeyName, FALSE);
		if (!ret)
		{
			_tprintf (_T ("Registry::Create(%s) failed.\n"), lpctszSubKeyName);
			break;
		}
		else
		{
			_tprintf (_T ("Registry::Create(%s) success.\n"), lpctszSubKeyName);
		}

		// -----------------------------------------
		// サブキー2を作成
		// -----------------------------------------

		Registry registry2;
		lpctszSubKeyName = _T ("TEST2");

		ret = registry2.Create (registry.GetKey (), lpctszSubKeyName, FALSE);
		if (!ret)
		{
			_tprintf (_T ("Registry::Create(%s) failed.\n"), lpctszSubKeyName);
			break;
		}
		else
		{
			_tprintf (_T ("Registry::Create(%s) success.\n"), lpctszSubKeyName);
		}

		// -----------------------------------------
		// サブキー1に値を設定
		// -----------------------------------------

		lpctszEntry = _T ("LOCALAPPDATA");
		LPCTSTR lpctszLocalAppData = _T ("%LOCALAPPDATA%");
		ZeroMemory (tszValue, sizeof (tszValue));
		TCHAR tszExpandPath[MAX_PATH];
		DWORD dwResponse;

		ret = registry1.QueryValue (lpctszEntry, eType, (LPBYTE)tszValue, 1024);
		if (!ret)
		{
			_tprintf (_T ("Registry::QueryValue(%s) failed. Try to create.\n"), lpctszEntry);

			DWORD dwDataLen = static_cast<DWORD>(_tcslen (lpctszLocalAppData) * sizeof (TCHAR));

			ret = registry1.SetValue (lpctszEntry, RegistryValueTypes::EXPAND_SZ, (LPBYTE)lpctszLocalAppData, dwDataLen);
			if (!ret)
			{
				_tprintf (_T ("Registry::SetValue(%s) <- %s failed.\n"), lpctszEntry, lpctszLocalAppData);
				break;
			}
			else
			{
				ret = registry1.QueryValue (lpctszEntry, eType, (LPBYTE)tszValue, 1024);
				if (!ret)
				{
					_tprintf (_T ("Registry::QueryValue(%s) failed.\n"), lpctszEntry);
					break;
				}
				else
				{
					dwResponse = ::ExpandEnvironmentStrings (tszValue, tszExpandPath, MAX_PATH);
					if (dwResponse > 0)
					{
						_tprintf (_T ("Registry::QueryValue[%d](%s) = %s. expanded %s.\n"),
								 eType, lpctszEntry, tszValue, tszExpandPath);
					}
					else
					{
						_tprintf (_T ("ExpandEnvironmentStrings() failed.\n"));
					}
				}
			}
		}
		else
		{
			if (eType == RegistryValueTypes::EXPAND_SZ)
			{
				dwResponse = ::ExpandEnvironmentStrings (tszValue, tszExpandPath, MAX_PATH);
				if (dwResponse > 0)
				{
					_tprintf (_T ("Registry::QueryValue[%d](%s) = %s. expanded %s.\n"),
							 eType, lpctszEntry, tszValue, tszExpandPath);
				}
				else
				{
					_tprintf (_T ("ExpandEnvironmentStrings() failed.\n"));
				}
			}
			else
			{
				_tprintf (_T ("Registry::QueryValue[%d](%s) = %s.\n"), eType, lpctszEntry, tszValue);
			}
		}

		// -----------------------------------------
		// サブキー1を削除
		// -----------------------------------------

		ret = registry.DeleteKey (lpctszSubKeyName);
		if (!ret)
		{
			_tprintf (_T ("Registry::DeleteKey(%s) failed.\n"), lpctszSubKeyName);
			break;
		}
		else
		{
			_tprintf (_T ("Registry::DeleteKey(%s) success.\n"), lpctszSubKeyName);
		}

		// -----------------------------------------
		// サブキー1をクローズ
		// -----------------------------------------

		ret = registry1.Close ();
		if (!ret)
		{
			_tprintf (_T ("Registry::Close(\"Test1\") failed.\n"));
			break;
		}
		else
		{
			_tprintf (_T ("Registry::Close(\"Test1\") success.\n"));
		}

		// -----------------------------------------
		// サブキー2をクローズ
		// -----------------------------------------

		ret = registry2.Close ();
		if (!ret)
		{
			_tprintf (_T ("Registry::Close(\"Test2\") failed.\n"));
			break;
		}
		else
		{
			_tprintf (_T ("Registry::Close(\"Test2\") success.\n"));
		}

		// -----------------------------------------
		// キーをクローズ
		// -----------------------------------------

		ret = registry.Close ();
		if (!ret)
		{
			_tprintf (_T ("Registry::Close() failed.\n"));
			break;
		}
		else
		{
			_tprintf (_T ("Registry::Close() success.\n"));
		}

		// -----------------------------------------
		// 完全消去
		// -----------------------------------------

		lpctszKeyName = _T ("SOFTWARE");
		Registry registry0;

		ret = registry0.Open (hKeyRoot, lpctszKeyName);
		if (!ret)
		{
			_tprintf (_T ("Registry::Open(%s) failed.\n"), lpctszKeyName);
			return -1;
		}
		else
		{
			_tprintf (_T ("Registry::Open(%s) success.\n"), lpctszKeyName);
		}

		lpctszSubKeyName = _T ("Alternate");

		ret = registry0.DeleteTree (lpctszSubKeyName);
		if (!ret)
		{
			_tprintf (_T ("Registry::DeleteTree(%s) failed.\n"), lpctszSubKeyName);
		}
		else
		{
			_tprintf (_T ("Registry::DeleteTree(%s) success.\n"), lpctszSubKeyName);
		}

	} while (0);

	return ERROR_SUCCESS;
}
