//! @file   ServiceCore.h
//! @brief  �T�[�r�X���s�N���X
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "ServiceCommon.hpp"
#include "Event.hpp"
#include "EventLogger.h"
#include "IService.h"
#include "ServiceInfo.h"

//! @brief �T�[�r�X���s�N���X
class DECLSPEC ServiceCore final
{
public:
	//! @brief �R���X�g���N�^
	//! @param logger EventLogger�N���X
	ServiceCore (EventLogger& logger);

	//! @brief�@�f�X�g���N�^
	~ServiceCore ();

	//! @brief  main()����̌Ăяo���ɑΉ�����֐�
	//! @param fnServiceMain    �T�[�r�X���C���֐�
	//! @param fnHandler        �T�[�r�X����֐�
	//! @param pService         �񋟃T�[�r�X�N���X
	//! @param si               �T�[�r�X�Ɋ֐����\����
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Entry (
		LPSERVICE_MAIN_FUNCTION fnServiceMain,
		LPHANDLER_FUNCTION fnHandler,
		IService* pService,
		ServiceInfo* si);

	//! @brief �T�[�r�X���C���֐�����̌Ăяo���ɑΉ�����֐�
	//! @param dwArgc       �T�[�r�X�R�[�����̈����̐�
	//! @param lptszArgv    �T�[�r�X�R�[�����̈���
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	VOID WINAPI Main (DWORD dwArgc, LPTSTR* lptszArgv);

	//! @brief �T�[�r�X����֐�����̌Ăяo���ɑΉ�����֐�
	//! @param dwControlCode ����R�[�h
	//! @return VOID
	VOID WINAPI Handler (DWORD dwControlCode);

private:
	//! @brief ����������
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Init ();

	//! @brief �J�n����
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Start ();

	//! @brief ��~����
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Stop ();

	//! @brief ���f����
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Suspend ();

	//! @brief �ĊJ����
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Resume ();

	//! @brief �I������
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Exit ();

	//! @brief �T�[�r�X�̌��݂̏�Ԃ�ݒ�
	//! @param dwCurrentState   ���ꂩ��J�ڂ�����X�e�[�^�X
	//!   - SERVICE_STOPPED(1)
	//!   - SERVICE_START_PENDING(2)
	//!   - SERVICE_STOP_PENDING(3)
	//!   - SERVICE_RUNNING(4)
	//!   - SERVICE_CONTINUE_PENDING(5)
	//!   - SERVICE_PAUSE_PENDING(6)
	//!   - SERVICE_PAUSED(7)
	//! @param dwWin32ExitCode
	//!   �T�[�r�X���J�n�܂��͒�~���Ă���Ƃ���
	//!   ��������G���[��񍐂��邽�߂Ɏg�p����
	//!   �G���[�R�[�h
	//! @param dwWaitHint
	//!   �ۗ����̊J�n�A��~�A�ꎞ��~�A�܂��͌p����
	//!   ����ɕK�v�Ȑ��莞�� (�~���b�P��)
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI ReportStatus (
		DWORD dwCurrentState,
		DWORD dwWin32ExitCode = NO_ERROR,
		DWORD dwWaitHint = MAX_STATE_TRANSITION_TIME);

	//! @brief �T�[�r�X����֐�����̐���R�[�h
	DWORD _dwControlCode;

	ServiceStatusHandler _handler;
	ServiceStatus _status;

	Event _event;
	EventLogger& _logger;

	//! @brief �T�[�r�X����֐�
	LPHANDLER_FUNCTION _fnHandler;

	IService* _pService;
	ServiceInfo* _pSI;
};

