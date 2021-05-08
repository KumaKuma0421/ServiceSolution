// ------------------------------------------------------------------------------------------------
// Main.cpp : このファイルには 'main' 関数が含まれています。
// ------------------------------------------------------------------------------------------------

#include "pch.h"
#include "Main.h"

HANDLE g_hCategoryResource = nullptr;
HANDLE g_hMessageResource = nullptr;

int _tmain(int argc, TCHAR** argv)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    _tsetlocale(LC_ALL, _T("Japanese"));

    HANDLE hEventLog = nullptr;
    DWORD dwStatus = ERROR_SUCCESS;
    DWORD dwBytesToRead = 0;
    DWORD dwBytesRead = 0;
    DWORD dwMinimumBytesToRead = 0;
    LPBYTE lpbyBuffer = nullptr;
    LPBYTE lpbyTemp = nullptr;
    TCHAR tszCategoryMessageFile[MAX_PATH] = { 0 };
    TCHAR tszEventMessageFile[MAX_PATH] = { 0 };
    BOOL ret;

    // The source name (provider) must exist as a subkey of Application.
    hEventLog = ::OpenEventLog(nullptr, SOURCE_NAME);
    if (nullptr == hEventLog)
    {
        _tprintf(_T("OpenEventLog failed with 0x%x.\n"), ::GetLastError());
        goto cleanup;
    }

    ret = GetResourceFiles(tszCategoryMessageFile, tszEventMessageFile);
    if (!ret)
    {
        _tprintf(_T("GetMessageResourceFile failed.\n"));
        goto cleanup;
    }

    // Get the DLL that contains the string resources for the provider.
    if (_tcslen(tszCategoryMessageFile) > 0)
    {
        g_hCategoryResource = GetMessageResources(tszCategoryMessageFile);
        if (nullptr == g_hCategoryResource)
        {
            _tprintf(_T("GetMessageResources failed.\n"));
            goto cleanup;
        }
    }

    // Get the DLL that contains the string resources for the provider.
    if (_tcslen(tszEventMessageFile) > 0)
    {
        g_hMessageResource = GetMessageResources(tszEventMessageFile);
        if (nullptr == g_hMessageResource)
        {
            _tprintf(_T("GetMessageResources failed.\n"));
            goto cleanup;
        }
    }

    // Allocate an initial block of memory used to read event records. The number 
    // of records read into the buffer will vary depending on the size of each event.
    // The size of each event will vary based on the size of the user-defined
    // data included with each event, the number and length of insertion 
    // strings, and other data appended to the end of the event record.
    dwBytesToRead = MAX_RECORD_BUFFER_SIZE;
    lpbyBuffer = (LPBYTE)::malloc(dwBytesToRead);
    if (nullptr == lpbyBuffer)
    {
        _tprintf(_T("Failed to allocate the initial memory for the record buffer.\n"));
        goto cleanup;
    }

    // Read blocks of records until you reach the end of the log or an 
    // error occurs. The records are read from newest to oldest. If the buffer
    // is not big enough to hold a complete event record, reallocate the buffer.
    while (ERROR_SUCCESS == dwStatus)
    {
        if (!::ReadEventLog(hEventLog,
            EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ,
            0,
            lpbyBuffer,
            dwBytesToRead,
            &dwBytesRead,
            &dwMinimumBytesToRead))
        {
            dwStatus = ::GetLastError();
            if (ERROR_INSUFFICIENT_BUFFER == dwStatus)
            {
                dwStatus = ERROR_SUCCESS;

                lpbyTemp = (PBYTE)::realloc(lpbyBuffer, dwMinimumBytesToRead);
                if (nullptr == lpbyTemp)
                {
                    _tprintf(_T("Failed to reallocate the memory for the record buffer (%d bytes).\n"), dwMinimumBytesToRead);
                    goto cleanup;
                }

                lpbyBuffer = lpbyTemp;
                dwBytesToRead = dwMinimumBytesToRead;
            }
            else
            {
                if (ERROR_HANDLE_EOF != dwStatus)
                {
                    _tprintf(_T("ReadEventLog failed with %lu.\n"), dwStatus);
                    goto cleanup;
                }
            }
        }
        else
        {
            // Print the contents of each record in the buffer.
            DumpRecordsInBuffer(lpbyBuffer, dwBytesRead);
        }
    }

cleanup:

    if (hEventLog)
        ::CloseEventLog(hEventLog);

    if (lpbyBuffer)
        ::free(lpbyBuffer);
}


//
// レジストリからメッセージリソースDLLのファイル名を取得します。
// 
BOOL GetResourceFiles(LPTSTR lptszCategoryMessageFile, LPTSTR lptszEventMessageFile)
{
    Registry registry1;
    HKEY hKeyRoot = HKEY_LOCAL_MACHINE;
    TCHAR tszRootKey[MAX_PATH];
    TCHAR tszPath[MAX_PATH];
    RegistryValueTypes eType;
    BOOL ret;

    wsprintf(tszRootKey, _T("%s\\%s\\%s"), EVENT_ROOT, SOURCE_NAME, PROVIDER_NAME);

    ret = registry1.Open(hKeyRoot, tszRootKey, KEY_READ);
    if (!ret) return FALSE;

    ret = registry1.QueryValue(ENTRY_CATEGORY_MESSAGE_FILE, eType, (LPBYTE)tszPath, MAX_PATH);
    if (!ret) return FALSE;

    if (eType == RegistryValueTypes::EXPAND_SZ)
    {
        ::ExpandEnvironmentStrings(tszPath, lptszCategoryMessageFile, MAX_PATH);
    }
    else
    {
        _tcscpy_s(lptszCategoryMessageFile, MAX_PATH, tszPath);
    }

    ret = registry1.QueryValue(ENTRY_EVENT_MESSAGE_FILE, eType, (LPBYTE)tszPath, MAX_PATH);
    if (!ret) return FALSE;

    if (eType == RegistryValueTypes::EXPAND_SZ)
    {
        ::ExpandEnvironmentStrings(tszPath, lptszEventMessageFile, MAX_PATH);
    }
    else
    {
        _tcscpy_s(lptszEventMessageFile, MAX_PATH, tszPath);
    }

    return TRUE;
}

// Get the provider DLL that contains the string resources for the
// category strings, event message strings, and parameter insert strings.
// For this example, the path to the DLL is hardcoded but typically,
// you would read the CategoryMessageFile, EventMessageFile, and 
// ParameterMessageFile registry values under the source's registry key located 
// under \SYSTEM\CurrentControlSet\Services\Eventlog\Application in
// the HKLM registry hive. In this example, all resources are included in
// the same resource-only DLL.
HANDLE GetMessageResources(LPCTSTR lpctszResourceFile)
{
    HANDLE hResources = nullptr;

    hResources = ::LoadLibraryEx(lpctszResourceFile, nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
    if (nullptr == hResources)
    {
        _tprintf(_T("LoadLibrary failed with %lu.\n"), ::GetLastError());
    }

    return hResources;
}


// Loop through the buffer and print the contents of each record 
// in the buffer.
DWORD DumpRecordsInBuffer(LPBYTE lpbyBuffer, DWORD dwBytesRead)
{
    DWORD dwStatus = ERROR_SUCCESS;
    LPBYTE lpbyRecord = lpbyBuffer;
    LPBYTE lpbyEndOfRecords = lpbyBuffer + dwBytesRead;
    LPTSTR lptszMessage = nullptr;
    LPTSTR lptszFinalMessage = nullptr;
    TCHAR tszTimeStamp[MAX_TIMESTAMP_LEN];

    while (lpbyRecord < lpbyEndOfRecords)
    {
        // If the event was written by our provider, write the contents of the event.
        if (0 == ::_tcscmp(PROVIDER_NAME, (LPTSTR)(lpbyRecord + sizeof(EVENTLOGRECORD))))
        {
            PEVENTLOGRECORD p = (PEVENTLOGRECORD)lpbyRecord;
            GetTimeStamp(((PEVENTLOGRECORD)lpbyRecord)->TimeGenerated, tszTimeStamp);
            _tprintf(_T("------------------------------------------------------------\n"));
            _tprintf(_T("time stamp: %s\n"), tszTimeStamp);
            _tprintf(_T("record number: %lu\n"), ((PEVENTLOGRECORD)lpbyRecord)->RecordNumber);
            _tprintf(_T("event type: %s\n"), lptszEventTypeNames[GetEventTypeName(((PEVENTLOGRECORD)lpbyRecord)->EventType)]);
            _tprintf(_T("event id: %d\n"), ((PEVENTLOGRECORD)lpbyRecord)->EventID & 0xFFFF);

            lptszMessage = GetMessageString(g_hCategoryResource, ((PEVENTLOGRECORD)lpbyRecord)->EventCategory, 0, nullptr);

            if (lptszMessage)
            {
                _tprintf(_T("event category: %d %s"), ((PEVENTLOGRECORD)lpbyRecord)->EventCategory, lptszMessage);
                ::LocalFree(lptszMessage);
                lptszMessage = nullptr;
            }

            lptszMessage = GetMessageString(g_hMessageResource, ((PEVENTLOGRECORD)lpbyRecord)->EventID,
                ((PEVENTLOGRECORD)lpbyRecord)->NumStrings, (LPTSTR)(lpbyRecord + ((PEVENTLOGRECORD)lpbyRecord)->StringOffset));

            if (lptszMessage)
            {
                dwStatus = ApplyParameterStringsToMessage(lptszMessage, lptszFinalMessage);

                _tprintf(_T("event message: %s"), (lptszFinalMessage) ? lptszFinalMessage : lptszMessage);
                ::LocalFree(lptszMessage);
                lptszMessage = nullptr;

                if (lptszFinalMessage)
                {
                    ::free(lptszFinalMessage);
                    lptszFinalMessage = nullptr;
                }
            }


            // To write the event data, you need to know the format of the data. In
            // this example, we know that the event data is a null-terminated string.
            if (((PEVENTLOGRECORD)lpbyRecord)->DataLength > 0)
            {
                _tprintf(_T("event data: %s\n"), (LPTSTR)(lpbyRecord + ((PEVENTLOGRECORD)lpbyRecord)->DataOffset));
            }

            _tprintf(_T("\n"));
        }

        lpbyRecord += ((PEVENTLOGRECORD)lpbyRecord)->Length;
    }

    return dwStatus;
}


// Get an dwIndex value to the lptszEventTypeNames array based on 
// the event type value.
DWORD GetEventTypeName(DWORD dwEventType)
{
    DWORD dwIndex = 0;

    switch (dwEventType)
    {
        case EVENTLOG_SUCCESS:
            dwIndex = 0;
            break;
        case EVENTLOG_ERROR_TYPE:
            dwIndex = 1;
            break;
        case EVENTLOG_WARNING_TYPE:
            dwIndex = 2;
            break;
        case EVENTLOG_INFORMATION_TYPE:
            dwIndex = 3;
            break;
        case EVENTLOG_AUDIT_SUCCESS:
            dwIndex = 4;
            break;
        case EVENTLOG_AUDIT_FAILURE:
            dwIndex = 5;
            break;
    }

    return dwIndex;
}


// Formats the specified message. If the message uses inserts, build
// the argument list to pass to FormatMessage.
LPTSTR GetMessageString(HANDLE hResource, DWORD dwMessageID, DWORD dwArgc, LPTSTR lptszArgv)
{
    LPTSTR lptszMessage = nullptr;
    DWORD dwFormatFlags =
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_FROM_HMODULE |
        FORMAT_MESSAGE_ALLOCATE_BUFFER;
    DWORD_PTR* lpdwArgs = nullptr;
    LPTSTR lptszString = lptszArgv;

    // The insertion strings appended to the end of the event record
    // are an array of strings; however, FormatMessage requires
    // an array of addresses. Create an array of DWORD_PTRs based on
    // the count of strings. Assign the address of each string
    // to an element in the array (maintaining the same order).
    if (dwArgc > 0)
    {
        lpdwArgs = (DWORD_PTR*)::malloc(sizeof(DWORD_PTR) * dwArgc);
        if (lpdwArgs)
        {
            dwFormatFlags |= FORMAT_MESSAGE_ARGUMENT_ARRAY;

            for (DWORD i = 0; i < dwArgc; i++)
            {
                lpdwArgs[i] = (DWORD_PTR)lptszString;
                lptszString += _tcslen(lptszString) + 1;
            }
        }
        else
        {
            dwFormatFlags |= FORMAT_MESSAGE_IGNORE_INSERTS;
            _tprintf(_T("Failed to allocate memory for the insert string array.\n"));
        }
    }

    if (!::FormatMessage(dwFormatFlags,
        hResource,
        dwMessageID,
        0,
        (LPTSTR)&lptszMessage,
        0,
        (va_list*)lpdwArgs))
    {
        _tprintf(_T("Format message failed with %lu\n"), ::GetLastError());
    }

    if (lpdwArgs)
        ::free(lpdwArgs);

    return lptszMessage;
}

// If the message string contains parameter insertion strings (for example, %%4096),
// you must perform the parameter substitution yourself. To get the parameter message 
// string, call FormatMessage with the message identifier found in the parameter insertion 
// string (for example, 4096 is the message identifier if the parameter insertion string
// is %%4096). You then substitute the parameter insertion string in the message 
// string with the actual parameter message string. 
DWORD ApplyParameterStringsToMessage(LPCTSTR lptszMessage, LPTSTR& lptszFinalMessage)
{
    DWORD dwStatus = ERROR_SUCCESS;
    DWORD dwParameterCount = 0;  // Number of insertion strings found in lptszMessage
    size_t cbBuffer = 0;         // Size of the buffer in bytes
    size_t cchBuffer = 0;        // Size of the buffer in characters
    size_t cchParameters = 0;    // Number of characters in all the parameter strings
    size_t cch = 0;
    DWORD i = 0;
    LPTSTR* lptszStartingAddresses = nullptr;  // Array of pointers to the beginning of each parameter string in lptszMessage
    LPTSTR* lptszEndingAddresses = nullptr;    // Array of pointers to the end of each parameter string in lptszMessage
    LPDWORD lpdwParameterIDs = nullptr;        // Array of parameter identifiers found in lptszMessage
    LPTSTR* lptszParameters = nullptr;         // Array of the actual parameter strings
    LPTSTR lptszTempMessage = (LPTSTR)lptszMessage;
    LPTSTR lptszTempFinalMessage = nullptr;

    // Determine the number of parameter insertion strings in lptszMessage.
    while (lptszTempMessage = ::_tcschr(lptszTempMessage, L'%'))
    {
        dwParameterCount++;
        lptszTempMessage++;
    }

    // If there are no parameter insertion strings in lptszMessage, return.
    if (0 == dwParameterCount)
    {
        lptszFinalMessage = nullptr;
        goto cleanup;
    }

    // Allocate an array of pointers that will contain the beginning address 
    // of each parameter insertion string.
    cbBuffer = sizeof(LPTSTR) * dwParameterCount;
    lptszStartingAddresses = (LPTSTR*)::malloc(cbBuffer);
    if (nullptr == lptszStartingAddresses)
    {
        _tprintf(_T("Failed to allocate memory for lptszStartingAddresses.\n"));
        dwStatus = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    ::RtlZeroMemory(lptszStartingAddresses, cbBuffer);

    // Allocate an array of pointers that will contain the ending address (one
    // character past the of the identifier) of the each parameter insertion string.
    lptszEndingAddresses = (LPTSTR*)::malloc(cbBuffer);
    if (nullptr == lptszEndingAddresses)
    {
        _tprintf(_T("Failed to allocate memory for lptszEndingAddresses.\n"));
        dwStatus = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    ::RtlZeroMemory(lptszEndingAddresses, cbBuffer);

    // Allocate an array of pointers that will contain pointers to the actual
    // parameter strings.
    lptszParameters = (LPTSTR*)::malloc(cbBuffer);
    if (nullptr == lptszParameters)
    {
        _tprintf(_T("Failed to allocate memory for lptszEndingAddresses.\n"));
        dwStatus = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    ::RtlZeroMemory(lptszParameters, cbBuffer);

    // Allocate an array of DWORDs that will contain the message identifier
    // for each parameter.
    lpdwParameterIDs = (DWORD*)::malloc(cbBuffer);
    if (nullptr == lpdwParameterIDs)
    {
        _tprintf(_T("Failed to allocate memory for lpdwParameterIDs.\n"));
        dwStatus = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    ::RtlZeroMemory(lpdwParameterIDs, cbBuffer);

    // Find each parameter in lptszMessage and get the pointer to the
    // beginning of the insertion string, the end of the insertion string,
    // and the message identifier of the parameter.
    lptszTempMessage = (LPTSTR)lptszMessage;
    while (lptszTempMessage = ::_tcschr(lptszTempMessage, L'%'))
    {
        if (::isdigit(*(lptszTempMessage + 1)))
        {
            lptszStartingAddresses[i] = lptszTempMessage;

            lptszTempMessage++;
            lpdwParameterIDs[i] = (DWORD)::_wtoi(lptszTempMessage);

            while (::isdigit(*++lptszTempMessage))
                ;

            lptszEndingAddresses[i] = lptszTempMessage;

            i++;
        }
    }

    // For each parameter, use the message identifier to get the
    // actual parameter string.
    for (DWORD i = 0; i < dwParameterCount; i++)
    {
        lptszParameters[i] = GetMessageString(g_hMessageResource, lpdwParameterIDs[i], 0, nullptr); // TODO:g_hMessageResourceは暫定措置
        if (nullptr == lptszParameters[i])
        {
            _tprintf(_T("GetMessageString could not find parameter string for insert %lu.\n"), i);
            dwStatus = ERROR_INVALID_PARAMETER;
            goto cleanup;
        }

        cchParameters += ::_tcslen(lptszParameters[i]);
    }

    // Allocate enough memory for lptszFinalMessage based on the length of lptszMessage
    // and the length of each parameter string. The lptszFinalMessage buffer will contain 
    // the completed parameter substitution.
    lptszTempMessage = (LPTSTR)lptszMessage;
    cbBuffer = (_tcslen(lptszMessage) + cchParameters + 1) * sizeof(WCHAR);
    lptszFinalMessage = (LPTSTR)::malloc(cbBuffer);
    if (nullptr == lptszFinalMessage)
    {
        _tprintf(_T("Failed to allocate memory for lptszFinalMessage.\n"));
        dwStatus = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    ::RtlZeroMemory(lptszFinalMessage, cbBuffer);
    cchBuffer = cbBuffer / sizeof(WCHAR);
    lptszTempFinalMessage = lptszFinalMessage;

    // Build the final message string.
    for (DWORD i = 0; i < dwParameterCount; i++)
    {
        // Append the segment from lptszMessage. In the first iteration, this is "8 " and in the
        // second iteration, this is " = 2 ".
        _tcsncpy_s(lptszTempFinalMessage, cchBuffer, lptszTempMessage, cch = (lptszStartingAddresses[i] - lptszTempMessage));
        lptszTempMessage = lptszEndingAddresses[i];
        cchBuffer -= cch;

        // Append the parameter string. In the first iteration, this is "quarts" and in the
        // second iteration, this is "gallons"
        lptszTempFinalMessage += cch;
        _tcscpy_s(lptszTempFinalMessage, cchBuffer, lptszParameters[i]);
        cchBuffer -= cch = _tcslen(lptszParameters[i]);

        lptszTempFinalMessage += cch;
    }

    // Append the last segment from lptszMessage, which is ".".
    _tcscpy_s(lptszTempFinalMessage, cchBuffer, lptszTempMessage);

cleanup:

    if (ERROR_SUCCESS != dwStatus)
        lptszFinalMessage = (LPTSTR)lptszMessage;

    if (lptszStartingAddresses)
        ::free(lptszStartingAddresses);

    if (lptszEndingAddresses)
        ::free(lptszEndingAddresses);

    if (lpdwParameterIDs)
        ::free(lpdwParameterIDs);

    for (DWORD i = 0; i < dwParameterCount; i++)
    {
        if (lptszParameters[i])
            ::LocalFree(lptszParameters[i]);
    }

    return dwStatus;
}


// Get a string that contains the time stamp of when the event 
// was generated.
VOID GetTimeStamp(const DWORD cdwTime, TCHAR tszDisplayString[])
{
    ULONGLONG ullTimeStamp = 0;
    ULONGLONG ullSecsTo1970 = 116444736000000000;
    SYSTEMTIME st;
    FILETIME ft;
    FILETIME ftLocal;

    ullTimeStamp = Int32x32To64(cdwTime, 10000000) + ullSecsTo1970;
    ft.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
    ft.dwLowDateTime = (DWORD)(ullTimeStamp & 0xFFFFFFFF);

    ::FileTimeToLocalFileTime(&ft, &ftLocal);
    ::FileTimeToSystemTime(&ftLocal, &st);
    ::StringCchPrintf(
        tszDisplayString,
        MAX_TIMESTAMP_LEN,
        _T("%04d/%02d/%02d %.2d:%.2d:%.2d"),
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}