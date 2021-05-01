//! @file   Main.h
//! @brief  �T���v���T�[�r�X ���̂Q
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "framework.h"
#include "ServiceCommon.hpp"
#include "ServiceCore.h"
#include "ServiceCommand.h"
#include "EventLogger.h"

#define SERVICE_NAME _T("ServiceApplication2")
#define SERVICE_DISPLAY_NAME _T("�T���v��2�̃T�[�r�X")
#define SERVICE_DESCRIPTION _T("����̓T���v��2�̃T�[�r�X�v���O�����ł��B")

//! @brief
//!   ���C���֐�
//! @details
//!   �n�r����̃T�[�r�X�N�����A�R�}���h�v�����v�g����̃T�[�r�X�����
//!   ���̊֐�����J�n���܂��B
//! @param argc �����̐�
//! @param argv ����
//! @return BOOL
//! @retval ERROR_SUCCESS(����)
//! @retval -1(���s)
int WINAPI _tmain(int argc, TCHAR** argv);

//! @brief
//!   �T�[�r�X�֐�
//! @details
//!   �n�r����̃T�[�r�X�J�n�v���́A��������̊J�n�ƂȂ�܂��B
//!   main�֐��ł�StartServiceCtrlDispatcher�֐��R�[�����_�@��
//!   �Ăяo����܂��B
//! @param dwArgc �����̐�
//! @param lptszArgv ����
//! @return VOID
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lptszArgv);

//! @brief
//!   �T�[�r�X����֐�
//! @details
//!   �n�r����̃T�[�r�X����v���ɑΉ����܂��B
//! @param dwControlCode
//!   - SERVICE_CONTROL_STOP
//!   - SERVICE_CONTROL_SHUTDOWN
//!   - SERVICE_CONTROL_PAUSE
//!   - SERVICE_CONTROL_PRESHUTDOWN
//!   - SERVICE_CONTROL_CONTINUE
//! @return VOID
VOID WINAPI CtrlHandler(DWORD dwControlCode);
