//! @file	ServiceInfo.cpp
//! @brief  サービス情報クラス
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#include "pch.h"
#include "ServiceInfo.h"

ServiceInfo::ServiceInfo()
{
    ZeroMemory(_tszWorkDirectory, sizeof(_tszWorkDirectory));
    ZeroMemory(_tszServiceName, sizeof(_tszServiceName));
    ZeroMemory(_tszServiceDisplayName, sizeof(_tszServiceDisplayName));
    ZeroMemory(_tszServiceDescription, sizeof(_tszServiceDescription));
    ZeroMemory(_tszExecutePath, sizeof(_tszExecutePath));
    ZeroMemory(_tszDependentService, sizeof(_tszDependentService));
    _bAutoStart = FALSE;
}

ServiceInfo::ServiceInfo(LPCTSTR lpctszServiceName)
{
    ServiceInfo::ServiceInfo();
    _tcscpy_s(_tszServiceName, lpctszServiceName);
}

BOOL ServiceInfo::Initialize(LPCTSTR lpctszSectionName)
{
    BOOL ret = FALSE;
    DWORD dwRet = -1;
    TCHAR tszIniFile[MAX_PATH];

    do
    {
        dwRet = ::GetModuleFileName(nullptr, _tszExecutePath, MAX_PATH);
        if (!dwRet)
        {
            _tprintf(_T("GetModuleFileName()が失敗しました。\n"));
            break;
        }
        else
        {
            _tcscpy_s(_tszWorkDirectory, _tszExecutePath);
            for (int i = (int)_tcslen(_tszWorkDirectory); i >= 0; i--)
            {
                if (_tszWorkDirectory[i] == '\\')
                {
                    _tszWorkDirectory[i] = '\0';
                    break;
                }
            }
        }

        wsprintf(tszIniFile, _T("%s\\%s"), _tszWorkDirectory, INITIAL_FILE);

        if (!::PathFileExists(tszIniFile)) break;

        dwRet = ::GetPrivateProfileString(
                    lpctszSectionName,
                    _T("ServiceName"),
                    nullptr,
                    _tszServiceName,
                    MAX_PATH,
                    tszIniFile);
        if (dwRet == 0) break;

        dwRet = ::GetPrivateProfileString(
                    lpctszSectionName,
                    _T("DisplayName"),
                    nullptr,
                    _tszServiceDisplayName,
                    MAX_PATH,
                    tszIniFile);
        if (dwRet == 0) break;

        dwRet = ::GetPrivateProfileString(
                    lpctszSectionName,
                    _T("Description"),
                    nullptr,
                    _tszServiceDescription,
                    MAX_PATH,
                    tszIniFile);
        if (dwRet == 0) break;

        // optional
        dwRet = ::GetPrivateProfileString(
                    lpctszSectionName,
                    _T("DependentService"),
                    nullptr,
                    _tszDependentService,
                    MAX_PATH,
                    tszIniFile);

        TCHAR tszTemp[MAX_PATH];

        dwRet = ::GetPrivateProfileString(
                    lpctszSectionName,
                    _T("AutoStart"),
                    nullptr,
                    tszTemp,
                    MAX_PATH,
                    tszIniFile);
        if (dwRet == 0) break;

        if (_tcsicmp(tszTemp, _T("TRUE")) == 0)
        {
            _bAutoStart = TRUE;
        }
        else
        {
            _bAutoStart = FALSE;
        }

        ret = TRUE;

    } while (0);

    return ret;
}