//! @file   Registry.cpp
//! @brief  レジストリに関するWindowsAPIを集約したクラス
//! @author kumakuma0421@gmail.com
//! @date   2021.03.28

#include "pch.h"
#include "Registry.h"

Registry::Registry()
{
    _hkRoot = nullptr;
    _hkTarget = nullptr;
}

Registry::~Registry()
{
    if (_hkTarget != nullptr) Close();
}

BOOL Registry::Create(HKEY hkRoot, LPCTSTR lpctszSubKey, BOOL bVoratile)
{
    LSTATUS ret;
    DWORD dwReserved = 0;
    LPTSTR lptszClass = nullptr;
    DWORD dwOptions = bVoratile ? REG_OPTION_VOLATILE : REG_OPTION_NON_VOLATILE;
    REGSAM samDesired = KEY_ALL_ACCESS;
    LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr;
    DWORD dwDisposition = 0;

    ret = ::RegCreateKeyEx(
        hkRoot,
        lpctszSubKey,
        dwReserved,
        lptszClass,
        dwOptions,
        samDesired,
        lpSecurityAttributes,
        &_hkTarget,
        &dwDisposition
    );

    if (ret == ERROR_SUCCESS)
    {
        _hkRoot = hkRoot;
    }

    return ret == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL Registry::Open(HKEY hkRoot, LPCTSTR lpctszSubKey)
{
    LSTATUS ret;
    DWORD dwOptions = 0;
    REGSAM samDesired = KEY_ALL_ACCESS;

    ret = ::RegOpenKeyEx(
        hkRoot,
        lpctszSubKey,
        dwOptions,
        samDesired,
        &_hkTarget);

    if (ret == ERROR_SUCCESS)
    {
        _hkRoot = hkRoot;
    }

    return ret == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL Registry::Query()
{
    LSTATUS ret;

    TCHAR tszClass[MAX_PATH];
    DWORD dwClass = MAX_PATH;
    LPDWORD lpdwReserved = nullptr;
    DWORD dwSubKeys = 0;
    DWORD dwMaxSubKeyLen = 0;
    DWORD dwMaxClassLen = 0;
    DWORD dwValues = 0;
    DWORD dwMaxValueNameLen = 0;
    DWORD dwMaxValueLen = 0;
    DWORD dwSecurityDescriptor = 0;
    FILETIME ftLastWriteTime = { 0, 0 };

    ret = ::RegQueryInfoKey(
        _hkTarget,
        tszClass,
        &dwClass,
        lpdwReserved,
        &dwSubKeys,
        &dwMaxSubKeyLen,
        &dwMaxClassLen,
        &dwValues,
        &dwMaxValueNameLen,
        &dwMaxValueLen,
        &dwSecurityDescriptor,
        &ftLastWriteTime
    );

    if (ret == ERROR_SUCCESS)
    {
        _tprintf(_T("RegQueryInfoKey() SubKeys = %d\n"), dwSubKeys);

        TCHAR tszKey[MAX_PATH];
        DWORD dwKeyLen = MAX_PATH;
        LPTSTR lptszClass = nullptr;
        DWORD dwClass = 0;

        for (DWORD i = 0; i < dwSubKeys; i++)
        {
            ret = ::RegEnumKeyEx(
                _hkTarget,
                i,
                tszKey,
                &dwKeyLen,
                lpdwReserved,
                lptszClass,
                &dwClass,
                &ftLastWriteTime
            );

            if (ret == ERROR_SUCCESS)
            {
                _tprintf(_T("RegEnumKeyEx() Key(%d) %s\n"), i + 1, tszKey);
            }

            dwKeyLen = MAX_PATH;
        }
    }
    else
    {
        _tprintf(_T("RegEnumKeyEx() failed.\n"));
    }

    if (ret == ERROR_SUCCESS && dwValues)
    {
        for (DWORD i = 0; i < dwValues; i++)
        {
            TCHAR tszValue[MAX_PATH];
            DWORD dwValueLen = MAX_PATH;
            DWORD dwType = 0;
            BYTE byData[1024];
            DWORD dwDataLen = 1024;

            ret = ::RegEnumValue(
                _hkTarget,
                i,
                tszValue,
                &dwValueLen,
                lpdwReserved,
                &dwType,
                byData,
                &dwDataLen
            );

            if (ret == ERROR_SUCCESS)
            {
                _tprintf(_T("RegEnumValue(%d) Value(%s) type:%d "), i + 1, tszValue, dwType);

                switch (dwType)
                {
                    case REG_SZ:
                    case REG_MULTI_SZ:
                    case REG_EXPAND_SZ:
                        _tprintf(_T("%s"), (LPCTSTR)byData);
                        break;

                    case REG_DWORD:
                    case REG_DWORD_BIG_ENDIAN:
                    {
                        union Byte4Value
                        {
                            DWORD dwValue;
                            TCHAR tszValue[4];
                        };
                        Byte4Value value{};
                        _tcscpy_s(value.tszValue, (LPCTSTR)byData);
                        _tprintf(_T("%d"), value.dwValue);
                    }
                    break;

                    case REG_QWORD:
                    {
                        union Byte8Value
                        {
                            LONG64 qwValue;
                            TCHAR tszValue[8];
                        };
                        Byte8Value value{};
                        _tcscpy_s(value.tszValue, (LPCTSTR)byData);
                        _tprintf(_T("%lld"), value.qwValue);
                    }
                    break;

                    default:
                        for (DWORD j = 0; j < dwDataLen; j++)
                        {
                            _tprintf(_T("%02X "), byData[j]);
                            if (j > 0 && j % 16 == 0) _tprintf(_T("\n"));
                        }
                        break;
                }

                _tprintf(_T("\n"));
            }

        }
    }

    return ret == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL Registry::QueryValue(
    LPCTSTR lpctszEntry,
    RegistryValueTypes eType,
    LPBYTE lpbyData,
    DWORD dwDataLen)
{
    LSTATUS ret;
    DWORD dwType = (DWORD)eType;
    LPDWORD lpdwReserved = nullptr;

    ret = ::RegQueryValueEx(
        _hkTarget,
        lpctszEntry,
        lpdwReserved,
        &dwType,
        lpbyData,
        &dwDataLen);

    return ret == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL Registry::SetValue(
    LPCTSTR lpctszEntry,
    RegistryValueTypes eType,
    LPBYTE lpbyData,
    DWORD dwDataLen)
{
    LSTATUS ret;
    DWORD dwType = (DWORD)eType;
    DWORD dwReserved = 0;

    ret = ::RegSetValueEx(
        _hkTarget,
        lpctszEntry,
        dwReserved,
        dwType,
        lpbyData,
        dwDataLen);

    return ret == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL Registry::DeleteTree(LPCTSTR lpctszSubKey)
{
    LSTATUS ret = ::RegDeleteTree(_hkTarget, lpctszSubKey);

    return ret == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL Registry::DeleteKey(LPCTSTR lpctszSubKey)
{
    REGSAM samDesired = 0;
    DWORD dwReserved = 0;

    LSTATUS ret = ::RegDeleteKeyEx(_hkTarget, lpctszSubKey, samDesired, dwReserved);

    return ret == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL Registry::DeleteValue(LPCTSTR lpctszEntry)
{
    LSTATUS ret = ::RegDeleteValue(_hkTarget, lpctszEntry);

    return ret == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL Registry::Flush()
{
    LSTATUS ret = ::RegFlushKey(_hkTarget);

    return ret == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL Registry::Close()
{
    LSTATUS ret = ::RegCloseKey(_hkTarget);

    return ret == ERROR_SUCCESS ? TRUE : FALSE;
}