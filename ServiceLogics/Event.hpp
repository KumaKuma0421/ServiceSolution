//! @file   Event.hpp
//! @brief	�C�x���g�Ɋւ���WindowsAPI���W�񂵂��N���X
//! @author	kumakuma0421@gmail.com
//! @date	2021.4.29

#pragma once

#include "pch.h"

//! @brief	�C�x���g�Ɋւ���WindowsAPI���W�񂵂��N���X
class DECLSPEC Event final
{
public:
    //! @brief �R���X�g���N�^
    Event()
    {
        _hEvent = nullptr;
    };

    //! �f�X�g���N�^
    ~Event()
    {
        if (_hEvent == nullptr) Close();
    }

    //! @brief  �C�x���g���쐬���܂��B
    //! @note   �蓮���Z�b�g�C�x���g���쐬���܂��B
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Create()
    {
        _hEvent = ::CreateEvent(
            nullptr,
            TRUE,  // manual reset event
            FALSE, // not signaled
            nullptr);

        return _hEvent == nullptr ? FALSE : TRUE;
    };

    //! @brief �C�x���g���I�[�v�����܂��B
    //! @param lpctszEventName �C�x���g��
    //! @return 
    BOOL WINAPI Open(LPCTSTR lpctszEventName)
    {
        _hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpctszEventName);

        return _hEvent == nullptr ? FALSE : TRUE;
    };

    //! @brief  �C�x���g���Z�b�g(�V�O�i����Ԃ�)���܂��B
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Set()
    {
        return ::SetEvent(_hEvent);
    };

    //! @brief  �C�x���g�����Z�b�g(��V�O�i����Ԃ�)���܂��B
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Reset()
    {
        return ::ResetEvent(_hEvent);
    };

    //! @brief	�V�O�i����ԂɂȂ�ƃu���b�N���������܂��B
    //! @return	�u���b�N�����̗��R
    //! @retval �ȉ��̒l���ԋp����܂��B
    //!   - �n���h�����V�O�i�����(WAIT_OBJECT_O)
    //!   - �n���h���I�u�W�F�N�g���I��(WAIT_ABANDONED)
    //!   - �^�C���A�E�g(WAIT_TIMEOUT)
    //!   - �G���[����(WAIT_FAILED)
    //!   - SetWaitableTimer()�̊֐��Ăяo������(WAIT_IO_COMPLETION)
    DWORD WINAPI Wait(DWORD dwMilliSeconds = INFINITE)
    {
        return ::WaitForSingleObject(_hEvent, dwMilliSeconds);
    };

    //! @brief	 �n���h�����N���[�Y���܂��B
    //! @return	 BOOL
    //! @retval	 ����(TRUE)
    //! @retval	 ���s(FALSE)
    BOOL WINAPI Close()
    {
        return ::CloseHandle(_hEvent);
    };

private:
    HANDLE _hEvent;
};
