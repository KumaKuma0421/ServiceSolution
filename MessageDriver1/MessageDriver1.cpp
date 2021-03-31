//
// MessageDriver1.cpp
//

#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include "resource.h"

#define MODULE_PATH _T(".\\MessageResource1.dll")

BOOL GetString(HINSTANCE hInstance, DWORD dwMessageID, LPTSTR lptszMsg, int iLen)
{
    int res = ::LoadString(hInstance, dwMessageID, lptszMsg, iLen);
    return res == 0 ? FALSE : TRUE;
}

VOID Pattern1()
{
    TCHAR tszMessage[512];

    HMODULE hModule = ::LoadLibrary(MODULE_PATH);
    if (hModule == nullptr)
    {
        _tprintf(_T("%sが見つかりません。"), MODULE_PATH);
        exit(-1);
    }

    HINSTANCE hInstance = GetModuleHandle(MODULE_PATH);
    if (hInstance == nullptr)
    {
        _tprintf(_T("%sが見つかりません。"), MODULE_PATH);
        exit(-1);
    }

    ULONGLONG ulStartTick = GetTickCount64();
    for (int i = 0; i < 10; i++)
    {
        for (DWORD dwNo = IDS_STRING1; dwNo <= IDS_STRING6540; dwNo++)
        {
            BOOL ret = GetString(hInstance, dwNo, tszMessage, 512);
            //_tprintf(_T("%d:%s\n"), dwNo, tszMessage);
        }
    }
    ULONGLONG ulFinishTick = GetTickCount64();

    _tprintf(_T("duration:%lld\n"), ulFinishTick - ulStartTick);
    FreeLibrary(hModule);
}

VOID Pattern2()
{
    TCHAR tszMessage[512];

    ULONGLONG ulStartTick = GetTickCount64();

    for (int i = 0; i < 10; i++)
    {
        for (DWORD dwNo = IDS_STRING1; dwNo <= IDS_STRING6540; dwNo++)
        {
            HMODULE hModule = ::LoadLibrary(MODULE_PATH);
            if (hModule == nullptr)
            {
                _tprintf(_T("%sが見つかりません。"), MODULE_PATH);
                exit(-1);
            }

            HINSTANCE hInstance = GetModuleHandle(MODULE_PATH);
            if (hInstance == nullptr)
            {
                _tprintf(_T("%sが見つかりません。"), MODULE_PATH);
                exit(-1);
            }
            BOOL ret = GetString(hInstance, dwNo, tszMessage, 512);
            //_tprintf(_T("%d:%s\n"), dwNo, tszMessage);

            FreeLibrary(hModule);
        }
    }

    ULONGLONG ulFinishTick = GetTickCount64();

    _tprintf(_T("duration:%lld\n"), ulFinishTick - ulStartTick);
}

int _tmain(int argc, TCHAR** argv)
{

    _tsetlocale(LC_ALL, _T("Japanese"));

    Pattern1();
    Pattern2();

    return ERROR_SUCCESS;
}
