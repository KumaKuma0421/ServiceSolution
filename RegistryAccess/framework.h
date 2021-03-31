#pragma once

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する

// Windows ヘッダー ファイル
#include <windows.h>
#include <tchar.h>
#include <winnt.h>
#include <winreg.h>

#ifdef REGISTRYACCESS_EXPORTS
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#pragma comment(lib, "RegistryAccess.lib")
#endif

#pragma comment(lib, "advapi32.lib")
