//! @file   ServiceCommand.h
//! @brief  �T�[�r�X�ɑ΂��鑀����W�񂵂��N���X
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "ServiceCommon.hpp"
#include "ServiceInfo.h"

#define COMMAND_INSTALL                   _T("/install")
#define COMMAND_STATUS                    _T("/status")
#define COMMAND_REMOVE                    _T("/remove")
#define COMMAND_ENABLE                    _T("/enable")
#define COMMAND_ONDEMAND                  _T("/ondemand")
#define COMMAND_DISABLE                   _T("/disable")
#define COMMAND_CHANGE_DELAYED_AUTO_START _T("/delayed")
#define COMMAND_START                     _T("/start")
#define COMMAND_STOP                      _T("/stop")
#define COMMAND_CHANGE_DESCRIPTION        _T("/description")

//! @brief TemplateAction()�����Ŏg�p����֐��|�C���^
typedef BOOL (WINAPI *TemplateMethod)(ServiceControl& sc, LPVOID lpvParam);

//! @brief �T�[�r�X�ɑ΂��鑀����W�񂵂��N���X
class DECLSPEC ServiceCommand final
{
public:
	//! @brief �R���X�g���N�^
	//! @param si �T�[�r�X���N���X
	ServiceCommand(ServiceInfo& si);

    //! @brief                  �R�}���h�����̃G���g���[�֐�
    //! @param lpctszCommand    �R�}���h��
    //! @param lpctszOption     �R�}���h�I�v�V����
    //! @return                 BOOL
    //! @retval                 TRUE(����)
    //! @retval                 FALSE(���s)
    BOOL WINAPI Command(LPCTSTR lpctszCommand, LPCTSTR lpctszOption = nullptr);

    //! @brief  �T�[�r�X�̃C���X�g�[������
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Install();

    //! @brief  �T�[�r�X�̏�ԕ\��
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Status();

    //! @brief  �T�[�r�X�̍폜����
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Remove();

    //! @brief  �����J�n/�蓮�J�n��ݒ�
    //! @param  bAuto TRUE(�����J�n) FALSE(�蓮�J�n)
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Enable(BOOL bAuto);

    //! @brief  �T�[�r�X�̖�����
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Disable();

    //! @brief  �T�[�r�X�̐���������ύX
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI ChangeConfig2Description();

    //! @brief  ����(�x���j��ݒ�
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI ChangeConfig2DelayedAutoStart();

    //! @brief �T�[�r�X�̊J�n
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Start();

    //! @brief  �T�[�r�X�̏I��
    //! @details
    //!   �ˑ�����T�[�r�X������ꍇ�A�������ɏI�������܂��B
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Stop();

private:
	//! @brief �B�����ꂽ�R���X�g���N�^
	ServiceCommand();

	//! @brief �R�}���h����̃v�����[�O�ƃG�s���[�O
	//! @param dwOpenParam �T�[�r�X�I�[�v�����̃p�����[�^
	//! @param method �v�����[�O�ƃG�r���[�O�̊ԂŎ��s���鏈��
	//! @param lpctszLastMessage �I�����O�ɏo�͂��郁�b�Z�[�W
	//! @param lpvParam �ǉ��p�����[�^
	//! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
	BOOL WINAPI TemplateAction(
		DWORD dwOpenParam,
		TemplateMethod method,
		LPCTSTR lpctszLastMessage,
		LPVOID lpvParam = nullptr);

    //! @brief ���݂̃T�[�r�X�X�e�[�^�X���o�͂��܂��B
    //! @param response ���݂̃X�e�[�^�X 
    //! @return VOID
    VOID WINAPI ShowCurrentStatus(QueryResponse response);

    //! @brief ���̃T�[�r�X�Ɉˑ����Ă���T�[�r�X�̒�~����
    //! @param sc ��~������T�[�r�X��ServiceControl
    //! @param lpvParam �p�����[�^
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI StopDependentServices(
        ServiceControl& sc,
        LPVOID lpvParam);
	
    ServiceInfo& _si;
};
