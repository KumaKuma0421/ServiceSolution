//! @file   Service1.h
//! @brief	�T�[�r�X��񋟂���N���X
//! @author	kumakuma0421@gmail.com
//! @date	2021.4.29

#pragma once

#include "Thread.h"
#include "EventLogger.h"
#include "IService.h"
#include "ServiceMessage.h"

//! @brief �T�[�r�X��񋟂���N���X
class Service1 : public Thread, public IService
{
public:
	//! @brief �R���X�g���N�^
	//! @param logger EventLogger
	Service1 (EventLogger& logger);

	//! @brief �T�[�r�X���J�n���܂��B
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Start ();

	//! @brief �T�[�r�X���~���܂��B
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Suspend ();

	//! @brief �T�[�r�X���ĊJ���܂��B
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Resume ();

	//! @brief �T�[�r�X���~���܂��B
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Stop ();

	//! @brief �T�[�r�X�̏I����ҋ@���܂��B
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Wait ();

private:
	//! @brief �B�����ꂽ�R���X�g���N�^
	Service1 ();

	//! @brief �T�[�r�X�J�n�֐�
	//! @return BOOL
	//! @retval TRUE(����)
	//! @retval FALSE(���s)
	BOOL WINAPI Run ();

	EventLogger& _logger;
};
