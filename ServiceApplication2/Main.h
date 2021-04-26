//
// Main.h
//

#pragma once

#include "framework.h"
#include "ServiceCommon.hpp"
#include "ServiceCore.h"
#include "ServiceCommand.h"
#include "EventLogger.h"

#define SERVICE_NAME _T("ServiceApplication2")
#define SERVICE_DISPLAY_NAME _T("�T���v��2�̃T�[�r�X")
#define SERVICE_DESCRIPTION _T("����̓T���v��2�̃T�[�r�X�v���O�����ł��B")

int WINAPI _tmain(int argc, TCHAR** argv);
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lptszArgv);
VOID WINAPI CtrlHandler(DWORD dwControlCode);
