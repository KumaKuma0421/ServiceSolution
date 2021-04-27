//
// framework.h
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する

#include <Windows.h>
#include <tchar.h>
#include <winsvc.h>
#include <libloaderapi.h>
#include <processthreadsapi.h>
#include <locale.h>
#include <stdarg.h>
#include <shlwapi.h>

#ifdef SERVICELOGICS_EXPORTS
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#pragma comment(lib, "ServiceLogics.lib")
#endif

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "Shlwapi.lib")