#include <Windows.h>
#include <tchar.h>
#include <locale.h>
#include <debugapi.h>
#include <stdio.h>
#include <exception>

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

BOOL GetErrorMessage (DWORD dwMessageID, LPTSTR lptszBuf, DWORD dwBufLen);
BOOL GetMessage (DWORD dwErrorCD, LPTSTR lptszBuf, DWORD dwBufLen, LPCTSTR* lpctszMessages);

int _tmain (int argc, TCHAR** argv)
{
	_tsetlocale (LC_ALL, _T ("Japanese"));

	int selection = 1;

	if (selection == 0)
	{
		TCHAR tszMsg[MAX_BUFF_SIZE];
		ZeroMemory (tszMsg, sizeof (tszMsg));

		for (DWORD dwNo = 0; dwNo < MAXDWORD; dwNo++)
		{
			BOOL ret = GetErrorMessage (dwNo, tszMsg, sizeof (tszMsg));
			if (ret)
				_tprintf (_T ("%u %s"), dwNo, tszMsg);
			else
				_tprintf (_T ("%u ----------\n"), dwNo);
		}
	}

	if (selection == 1)
	{
		TCHAR tszMsg[MAX_BUFF_SIZE];
		ZeroMemory (tszMsg, sizeof (tszMsg));
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
			_T ("TEST1"),
			_T ("TEST2"),
			_T ("TEST3"),
		};

		for (DWORD dwNo : adwKeys)
		{
			BOOL ret = GetMessage (dwNo, tszMsg, sizeof (tszMsg), lpctszParams);
			if (ret)
				_tprintf (_T ("%u %s"), dwNo, tszMsg);
			else
				_tprintf (_T ("%u ----------\n"), dwNo);
		}
	}
}

BOOL GetErrorMessage (DWORD dwErrorCD, LPTSTR lptszBuf, DWORD dwBufLen)
{
	DWORD dwRet = ::FormatMessage (
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		dwErrorCD,
		MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		lptszBuf,
		dwBufLen,
		nullptr);

	return dwRet > 0 ? TRUE : FALSE;
}

BOOL GetMessage (DWORD dwErrorCD, LPTSTR lptszBuf, DWORD dwBufLen, LPCTSTR* lpctszMessages)
{
	LPCTSTR lpctszLibFileName = _T ("C:\\Users\\User01\\source\\repos\\ConsoleSolution\\x64\\Debug\\sample.dll");
	HANDLE hFile = nullptr;
	DWORD dwFlags = DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE | FORMAT_MESSAGE_ARGUMENT_ARRAY;

	HMODULE hModule = ::LoadLibraryEx (lpctszLibFileName, hFile, dwFlags);

	DWORD dwRet = ::FormatMessage (
		FORMAT_MESSAGE_FROM_HMODULE,
		hModule,
		dwErrorCD,
		MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		lptszBuf,
		dwBufLen,
		(va_list*)&lpctszMessages);

	return dwRet > 0 ? TRUE : FALSE;
}
