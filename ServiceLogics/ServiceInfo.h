//! @file	ServiceInfo.h
//! @brief  �T�[�r�X���N���X
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "pch.h"

#define INITIAL_FILE _T("ServiceConfig.ini")

//! @brief �T�[�r�X���N���X
class DECLSPEC ServiceInfo
{
public:
	//! @brief �R���X�g���N�^
	ServiceInfo ();

	//! @brief �R���X�g���N�^
	//! @param lpctszServiceName �T�[�r�X��
	ServiceInfo (LPCTSTR lpctszServiceName);

	//! @brief ����������
	//! @param lpctszSectionName �T�[�r�X��
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL Initialize (LPCTSTR lpctszSectionName);

	//! @brief ���݂̉ғ����Ă��郂�W���[���̂���f�B���N�g�������擾
	//! @return ���݂̉ғ����Ă��郂�W���[���̂���f�B���N�g����
	LPCTSTR GetWorkDirectory () { return _tszWorkDirectory; };

	//! @brief �T�[�r�X�����擾
	//! @return �T�[�r�X��
	LPCTSTR GetName () { return _tszServiceName; };

	//! @brief �T�[�r�X�\�������擾
	//! @return �T�[�r�X�\����
	LPCTSTR GetDisplayName () { return _tszServiceDisplayName; };

	//! @brief �T�[�r�X���������擾
	//! @return �T�[�r�X������
	LPCTSTR GetDescription () { return _tszServiceDescription; };

	//! @brief �T�[�r�X�v���O�����t�@�C�������擾
	//! @return �T�[�r�X�v���O�����t�@�C���� 
	LPCTSTR GetExecutePath () { return _tszExecutePath; };

	//! @brief ���̃T�[�r�X���ˑ����Ă���T�[�r�X�����擾
	//! @return ���̃T�[�r�X���ˑ����Ă���T�[�r�X��
	LPCTSTR GetDependentService () { return _tszDependentService; };

	//! @brief �T�[�r�X�̊J�n���@���擾
	//! @return �T�[�r�X�̊J�n���@�i�����A�蓮�j
	DWORD GetStartType ()
	{
		return _bAutoStart ? SERVICE_AUTO_START : SERVICE_DEMAND_START;
	};

private:
	TCHAR _tszWorkDirectory[MAX_PATH];
	TCHAR _tszServiceName[MAX_PATH];
	TCHAR _tszServiceDisplayName[MAX_PATH];
	TCHAR _tszServiceDescription[MAX_PATH];
	TCHAR _tszExecutePath[MAX_PATH];
	TCHAR _tszDependentService[MAX_PATH];
	BOOL _bAutoStart;
};