//! @file   Thread.h
//! @brief	�X���b�h�Ɋւ���WindowsAPI���W�񂵂��N���X
//! @author	kumakuma0421@gmail.com
//! @date	2021.4.29

#pragma once

#include "Event.hpp"

//! @brief �X���b�h�Ɋւ���WindowsAPI���W�񂵂��N���X
class DECLSPEC Thread
{
public:
    //! @brief �R���X�g���N�^
    Thread();

    //! @brief �f�X�g���N�^
    virtual ~Thread();

    //! @brief �X���b�h�̋N��
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    virtual BOOL Start();

    //! @brief �X���b�h�̒�~
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    virtual BOOL Stop();

    //! @brief �X���b�h�̒��f
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    virtual BOOL Suspend();

    //! @brief �X���b�h�̍ĊJ
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    virtual BOOL Resume();

    //! @brief �X���b�h�����֐�
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    virtual BOOL Run() = 0;

    //! @brief �I���ҋ@
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    virtual BOOL Wait(DWORD dwMillisec = INFINITE);

protected:
    Event _event;

private:
    //! @brief �X���b�h�����֐�
    //! @param lpvParam ���ۂɃX���b�h�������s���C���X�^���X
    //! @return �X���b�h�֐�����̖߂�l
    static DWORD ThreadFunction(LPVOID lpvParam);

    HANDLE _hThread;
    DWORD _dwThread;
};