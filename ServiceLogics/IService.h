//! @file   IService.h
//! @brief  �T�[�r�X�񋟃v���W�F�N�g�ւ̃C���^�[�t�F�[�X�N���X
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "pch.h"

//! @brief  �T�[�r�X�񋟃v���W�F�N�g�ւ̃C���^�[�t�F�[�X�N���X
class IService
{
public:
	//! @brief �R���X�g���N�^
	IService () {};

	//! @brief �f�X�g���N�^
	virtual ~IService () {};

	//! @brief �����J�n�C���^�[�t�F�[�X
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	virtual BOOL Start () = 0;

	//! @brief �������f�C���^�[�t�F�[�X
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	virtual BOOL Suspend () = 0;

	//! @brief �����ĊJ�C���^�[�t�F�[�X
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	virtual BOOL Resume () = 0;

	//! @brief �����I���C���^�[�t�F�[�X
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	virtual BOOL Stop () = 0;

	//! @brief �����I���ҋ@�C���^�[�t�F�[�X
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	virtual BOOL Wait () = 0;
};