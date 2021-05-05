//
// Main.h
//

#pragma once

#include "framework.h"
#include "Registry.h"

//#define SOURCE_NAME             _T("ServiceApplication")
//#define PROVIDER_NAME           _T("ServiceApplication1")
#define SOURCE_NAME             _T("Application")
#define PROVIDER_NAME           _T("ESENT")
#define MAX_TIMESTAMP_LEN       23 + 1   // mm/dd/yyyy hh:mm:ss.mmm
#define MAX_RECORD_BUFFER_SIZE  0x10000  // 64K

#define EVENT_ROOT _T("SYSTEM\\CurrentControlSet\\Services\\EventLog")
#define ENTRY_CATEGORY_MESSAGE_FILE _T("CategoryMessageFile")
#define ENTRY_EVENT_MESSAGE_FILE _T("EventMessageFile")

BOOL GetResourceFiles(LPTSTR lptszCategoryMessageFile, LPTSTR lptszEventMessageFile);
HANDLE GetMessageResources(LPCTSTR lpctszResourceFile);
DWORD DumpRecordsInBuffer(LPBYTE lpbyBuffer, DWORD dwBytesRead);
DWORD GetEventTypeName(DWORD dwEventType);
LPWSTR GetMessageString(HANDLE hResource, DWORD dwMessageID, DWORD dwArgc, LPTSTR lptszArgv);
DWORD ApplyParameterStringsToMessage(LPCTSTR lptszMessage, LPTSTR& lptszFinalMessage);
VOID GetTimeStamp(const DWORD cdwTime, TCHAR tszDisplayString[]);

LPCTSTR lptszEventTypeNames[] =
{
    (LPTSTR)_T("Success"),
    (LPTSTR)_T("Error"), 
    (LPTSTR)_T("Warning"),
    (LPTSTR)_T("Informational"),
    (LPTSTR)_T("Audit Success"),
    (LPTSTR)_T("Audit Failure")
};
