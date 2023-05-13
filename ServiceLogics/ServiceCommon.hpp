//! @file   ServiceCommon.hpp
//! @brief  �T�[�r�X����N���X���W�񂵂��t�@�C��
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "ServiceInfo.h"

#define LOG_ROOT _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\ServiceApplication")

#define ENTRY_MAX_SIZE _T("MaxSize")
#define LOG_MAX_SIZE 0x1400000

#define ENTRY_RETENTION _T("Retention")
#define LOG_RETENTION 0

#define ENTRY_FILE _T("File")
#define LOG_FILE _T("%SystemRoot%\\System32\\Winevt\\Logs\\ServiceApplication.evtx")

#define EVENT_ROOT _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\ServiceApplication")

#define ENTRY_CATEGORY_COUNT _T("CategoryCount")
#define EVENT_CATEGORY_COUNT 4

#define ENTRY_CATEGORY_MESSAGE_FILE _T("CategoryMessageFile")
#define EVENT_CATEGORY_MESSAGE_FILE _T("ServiceMessage.dll")

#define ENTRY_EVENT_MESSAGE_FILE _T("EventMessageFile")
#define EVENT_EVENT_MESSAGE_FILE _T("ServiceMessage.dll")

#define ENTRY_TYPES_SUPPORTED _T("TypesSupported")
#define EVENT_TYPES_SUPPORTED 7

#define ERROR_REGISTRY_REGISTER _T("���W�X�g���̓o�^�Ɏ��s���܂����B")

#define MAX_MESSAGE_LEN 256
#define MAX_STATE_TRANSITION_TIME 3000

//! @brief ���b�Z�[�W���R���\�[���ɏo�͂��܂��B
//! @param lpctszMessage �o�͂��郁�b�Z�[�W
//! @param dwErrorCode GetLastError()�̏o�͒n
//! @param lpctszOption �ǉ��̃��b�Z�[�W
//! @return 
VOID WINAPI PrintMessage (
	LPCTSTR lpctszMessage,
	DWORD dwErrorCode = GetLastError (),
	LPCTSTR lpctszOption = nullptr);

//! @brief �蓮�A�����A�����̑I����
enum class BootSettings
{
	//! @brief �蓮
	OnDemand = SERVICE_DEMAND_START,

	//! @brief ����
	Auto = SERVICE_AUTO_START,

	//! @brief ����
	Disable = SERVICE_DISABLED
};

//! @brief �T�[�r�X�̏�ԑJ��
enum class QueryResponse
{
	//! @brief �G���[
	Error = -1,

	//! @brief �^�C���A�E�g
	Timeout = 0,

	//! @brief ��~
	Stopped = SERVICE_STOPPED,

	//! @brief �J�n��
	StartPending = SERVICE_START_PENDING,

	//! @brief ��~��
	StopPending = SERVICE_STOP_PENDING,

	//! @brief ���s��
	Running = SERVICE_RUNNING,

	//! @brief �ĊJ��
	ContinuePending = SERVICE_CONTINUE_PENDING,

	//! @brief ���f��
	PausePending = SERVICE_PAUSE_PENDING,

	//! @brief ���f
	Paused = SERVICE_PAUSED
};

//! @brief SC_HANDLER�������N���X�̃X�[�p�[�N���X
class DECLSPEC ServiceHandler
{
public:
	//! @brief �R���X�g���N�^
	ServiceHandler ()
	{
		_handle = nullptr;
	};

	//! @brief �f�X�g���N�^
	virtual ~ServiceHandler ()
	{
		if (_handle != nullptr) Close ();
	};

	//! @brief  �N���[�Y����
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	virtual BOOL WINAPI Close ()
	{
		return ::CloseServiceHandle (_handle);
	}

	//! @brief  �n���h���擾
	//! @return SC_HANDLE
	virtual SC_HANDLE WINAPI GetHandle () { return _handle; };

protected:
	SC_HANDLE _handle;
};

//! @brief �T�[�r�X�R���g���[���}�l�[�W��(SCM)�̑���p�N���X
class DECLSPEC ServiceControlManager final : public ServiceHandler
{
public:
	//! @brief  SCM�̃I�[�v��
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Open ()
	{
		_handle = ::OpenSCManager (
			nullptr,
			nullptr,
			SC_MANAGER_ALL_ACCESS);

		return _handle == nullptr ? FALSE : TRUE;
	};
};

//! @brief �ʂ̃T�[�r�X���Ǘ�����N���X
class DECLSPEC ServiceControl final : public ServiceHandler
{
public:
	//! @brief �R���X�g���N�^
	//! @param manager �T�[�r�X�}�l�[�W���N���X
	//! @param si �ʂ̃T�[�r�X�Ɋւ�����
	ServiceControl (ServiceControlManager & manager, ServiceInfo & si)
		: _manager (manager), _si (si)
	{};

	//! @brief �T�[�r�X�̍쐬
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Create ()
	{
		_handle = ::CreateService (
			_manager.GetHandle (),       // SCM database 
			_si.GetName (),              // name of service 
			_si.GetDisplayName (),       // service name to display 
			SERVICE_ALL_ACCESS,         // desired access 
			SERVICE_WIN32_OWN_PROCESS,  // service type 
			_si.GetStartType (),         // start type 
			SERVICE_ERROR_NORMAL,       // error control type 
			_si.GetExecutePath (),       // path to service's binary 
			nullptr,                    // no load ordering group 
			nullptr,                    // no tag identifier 
			_si.GetDependentService (),  // dependent service
			nullptr,                    // LocalSystem account 
			nullptr);                   // no password 

		return _handle == nullptr ? FALSE : TRUE;
	};

	//! @brief �T�[�r�X�̃I�[�v��
	//! @param dwDesiredAccess �I�[�v���̍ۂɕK�v�ȃA�N�Z�X��
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Open (DWORD dwDesiredAccess)
	{
		_handle = ::OpenService (
			_manager.GetHandle (),
			_si.GetName (),
			dwDesiredAccess);

		return _handle == nullptr ? FALSE : TRUE;
	};

	//! @brief �T�[�r�X�̐ݒ���擾
	//! @param lpQueryServiceConfig �T�[�r�X�ݒ���
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI QueryConfig (LPQUERY_SERVICE_CONFIG& lpQueryServiceConfig)
	{
		BOOL ret;
		DWORD dwBytesNeeded = 0;

		ret = ::QueryServiceConfig (_handle, nullptr, 0, &dwBytesNeeded);
		if (!ret && dwBytesNeeded)
		{
			lpQueryServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc (LPTR, dwBytesNeeded);
			ret = ::QueryServiceConfig (_handle, lpQueryServiceConfig, dwBytesNeeded, &dwBytesNeeded);
		}

		return ret;
	}

	//! @brief �T�[�r�X�̏�Ԏ擾
	//! @param status �T�[�r�X�̏��
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI QueryStatusEx (SERVICE_STATUS_PROCESS& status)
	{
		DWORD dwBytesNeeded;

		return ::QueryServiceStatusEx (
			_handle,                        // handle to service 
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE)&status,                // address of structure
			sizeof (SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded);                // size needed if buffer is too small
	}

	//! @brief �T�[�r�X�̏�Ԏ擾
	//! @param wait �J�ڒ��̏ꍇ�A�ҋ@����X�e�[�^�X
	//! @return QueryResponse
	QueryResponse WINAPI QueryStatus (QueryResponse wait);

	//! @brief �T�[�r�X�̊J�n
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Start ()
	{
		return ::StartService (_handle, 0, nullptr);
	}

	//! @brief �T�[�r�X�̒�~
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Stop ()
	{
		SERVICE_STATUS service_status = { 0, 0, 0, 0, 0, 0, 0 };

		return ::ControlService (_handle, SERVICE_CONTROL_STOP, &service_status);
	}

	//! @brief �T�[�r�X�̐ݒ�ύX
	//! @param bootType BootSettings
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI ChangeConfig (BootSettings bootType)
	{
		return ::ChangeServiceConfig (
			_handle,           // handle of service 
			SERVICE_NO_CHANGE, // service type: no change 
			(DWORD)bootType,   // service start type 
			SERVICE_NO_CHANGE, // error control: no change 
			nullptr,           // binary path: no change 
			nullptr,           // load order group: no change 
			nullptr,           // tag ID: no change 
			nullptr,           // dependencies: no change 
			nullptr,           // account name: no change 
			nullptr,           // password: no change 
			nullptr);          // display name: no change
	};

	//! @brief �T�[�r�X�̐������͂̕ύX
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI ChangeConfig2Description ()
	{
		SERVICE_DESCRIPTION service_description = { (LPTSTR)_si.GetDescription () };

		return ::ChangeServiceConfig2 (
			_handle,
			SERVICE_CONFIG_DESCRIPTION,
			&service_description);
	}

	//! @brief �T�[�r�X�̒x���J�n�ݒ�
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	//! @note   �����i�x���J�n�j�ɂ���
	//!   �ʏ�̎����J�n�T�[�r�X�̖�Q����ɊJ�n�������́B
	//!   ����������(�x���J�n)�Ȃ̂ŁA���O�Ɏ����ɂȂ��Ă��邱�Ƃ��O��B
	//! @sa https://togarasi.wordpress.com/2008/05/17/%E3%82%B5%E3%83%BC%E3%83%93%E3%82%B9%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%A0%E3%81%AE%E8%87%AA%E5%8B%95%E8%B5%B7%E5%8B%95%E3%82%92%E9%81%85%E5%BB%B6%E5%AE%9F%E8%A1%8C%E3%81%AB%E5%A4%89%E6%9B%B4/
	BOOL WINAPI ChangeConfig2DelayedAutoStart (BOOL bDelayed = TRUE)
	{
		SERVICE_DELAYED_AUTO_START_INFO service_delayed_auto_start_info = { bDelayed };

		return ::ChangeServiceConfig2 (
			_handle,
			SERVICE_CONFIG_DELAYED_AUTO_START_INFO,
			&service_delayed_auto_start_info);
	}

	//! @brief �T�[�r�X�̍폜
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Delete ()
	{
		return ::DeleteService (_handle);
	};

	//! @brief ���̃T�[�r�X�ɕR�Â�SCM�̃n���h�����擾
	//! @return ServiceControlManager�N���X
	ServiceControlManager& GetManager () { return _manager; }

private:
	//! @brief �B�����ꂽ�R���X�g���N�^
	ServiceControl ();

	ServiceControlManager& _manager;
	ServiceInfo& _si;
};

//! @brief �T�[�r�X����֐��̓o�^���s���N���X
class DECLSPEC ServiceStatusHandler final
{
public:
	//! @brief �R���X�g���N�^
	ServiceStatusHandler ()
	{
		_hService = nullptr;
	};

	//! @brief �f�X�g���N�^
	~ServiceStatusHandler ()
	{}

	//! @brief �T�[�r�X����֐��̓o�^����
	//! @param lpctszServiceName �T�[�r�X��
	//! @param lpfnCtrlHandler �T�[�r�X����֐�
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Init (LPCTSTR lpctszServiceName, LPHANDLER_FUNCTION lpfnCtrlHandler)
	{
		_hService = ::RegisterServiceCtrlHandler (lpctszServiceName, lpfnCtrlHandler);
		return _hService == nullptr ? FALSE : TRUE;
	};

	//! @brief �T�[�r�X����֐��o�^���̃n���h�����擾
	//! @return SERVICE_STATUS_HANDLE�n���h��
	SERVICE_STATUS_HANDLE WINAPI GetHandle () { return _hService; };

private:
	SERVICE_STATUS_HANDLE _hService;
};

//! @brief �T�[�r�X��Ԃ̑���N���X
class DECLSPEC ServiceStatus final
{
public:
	//! @brief �R���X�g���N�^
	ServiceStatus ()
	{
		_Status = { 0,0,0,0,0,0,0 };
	};

	//! @brief                  �T�[�r�X�̌��݂̏�Ԃ�ݒ�
	//! @param handler          ServiceStatusHandler
	//! @param dwCurrentState   ���ꂩ��J�ڂ�����X�e�[�^�X
	//!   - SERVICE_STOPPED(1)
	//!   - SERVICE_START_PENDING(2)
	//!   - SERVICE_STOP_PENDING(3)
	//!   - SERVICE_RUNNING(4)
	//!   - SERVICE_CONTINUE_PENDING(5)
	//!   - SERVICE_PAUSE_PENDING(6)
	//!   - SERVICE_PAUSED(7)
	//! @param dwWin32ExitCode
	//!   �T�[�r�X���J�n�܂��͒�~���Ă���Ƃ��ɔ�������G���[��
	//!   �񍐂��邽�߂Ɏg�p����G���[�R�[�h
	//! @param dwWaitHint
	//!   �ۗ����̊J�n�A��~�A�ꎞ��~�A�܂��͌p���̑���ɕK�v��
	//!   ���莞�� (�~���b�P��)
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI ReportStatus (
		ServiceStatusHandler handler,
		DWORD dwCurrentState,
		DWORD dwWin32ExitCode,
		DWORD dwWaitHint);

	//! @brief �T�[�r�X�̏�Ԏ擾
	//! @return SERVICE_STATUS�\����
	SERVICE_STATUS WINAPI GetStatus () { return _Status; };

private:
	SERVICE_STATUS _Status;
};
