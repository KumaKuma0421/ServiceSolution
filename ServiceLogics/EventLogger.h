//! @file   EventLogger.h
//! @brief  �C�x���g���O�ɏ������ރN���X
//! @author kumakuma0421@gmail.com
//! @date   2021.4.29

#pragma once

#include "ServiceMessage.h"

//! @brief �C�x���g���O�������݃N���X 
class DECLSPEC EventLogger final
{
public:
    //! @brief �R���X�g���N�^
    EventLogger()
    {
        _hEventSource = nullptr;
    };

    //! @brief �f�X�g���N�^
    ~EventLogger()
    {
        if (_hEventSource != nullptr) Exit();
    };

    //! @brief ����������
    //! @param lpctszServiceName �T�[�r�X��
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Init(LPCTSTR lpctszServiceName);

    //! @brief �I������
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Exit();

    //! @brief
    //!   �ėp�I�ȃ��O�o�͏���
    //! @param wEventType �ȉ��̒l��ݒ肵�܂��B
    //!   - EVENTLOG_SUCCESS
    //!   - EVENTLOG_ERROR_TYPE
    //!   - EVENTLOG_WARNING_TYPE
    //!   - EVENTLOG_INFORMATION_TYPE
    //! @param wEventCategory �ȉ��̒l��ݒ肵�܂��B
    //!   - CATEGORY_SERVICE_CORE
    //!   - CATEGORY_SERVICE_MANAGER
    //!   - CATEGORY_SERVICE_CONTROL
    //!   - CATEGORY_SERVICE
    //! @param dwEventID �ȉ��̒l��ݒ肵�܂��B
    //!   - SVC_SUCCESS_SYSTEM
    //!   - SVC_SUCCESS_RUNTIME
    //!   - SVC_SUCCESS_API
    //!   - SVC_SUCCESS_IO
    //!   - SVC_INFO_SYSTEM
    //!   - SVC_INFO_RUNTIME
    //!   - SVC_INFO_API
    //!   - SVC_INFO_IO
    //!   - SVC_INFO_TRACE
    //!   - SVC_WARNING_SYSTEM
    //!   - SVC_WARNING_RUNTIME
    //!   - SVC_WARNING_API
    //!   - SVC_WARNING_IO
    //!   - SVC_ERROR_SYSTEM
    //!   - SVC_ERROR_RUNTIME
    //!   - SVC_ERROR_API
    //!   - SVC_ERROR_IO
    //! @param wArgc    �p�����[�^�[�̌�
    //! @note
    //!   �σp�����[�^�͂��ׂĕ�����ł��B
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Log(
        WORD wEventType,
        WORD wEventCategory,
        DWORD dwEventID,
        WORD wArgc,
        ...);

    //! @brief
    //!   API�G���[�̃��O�o�͗p
    //! @param wCategory �ȉ��̒l��ݒ肵�܂��B
    //!   - CATEGORY_SERVICE_CORE
    //!   - CATEGORY_SERVICE_MANAGER
    //!   - CATEGORY_SERVICE_CONTROL
    //!   - CATEGORY_SERVICE
    //! @param dwErrorCode  GetLastError()�̒l 
    //! @param lpctszFunctionName �Ăяo�����̊֐���
    //! @param lpctszErrorAPI �G���[�𔭐�����API��
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI ApiError(
        WORD wCategory,
        DWORD dwErrorCode,
        LPCTSTR lpctszFunctionName,
        LPCTSTR lpctszErrorAPI);

    //! @brief
    //!   �����J�n���O�o�͗p
    //! @param wCategory �ȉ��̒l��ݒ肵�܂��B
    //!   - CATEGORY_SERVICE_CORE
    //!   - CATEGORY_SERVICE_MANAGER
    //!   - CATEGORY_SERVICE_CONTROL
    //!   - CATEGORY_SERVICE
    //! @param lpctszFunctionName   �Ăяo�����֐���(__FUNCTIONW__)
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI TraceStart(WORD wCategory, LPCTSTR lpctszFunctionName);

    //! @brief
    //!   �����I�����O�o�͗p
    //! @param wCategory �ȉ��̒l��ݒ肵�܂��B
    //!   - CATEGORY_SERVICE_CORE
    //!   - CATEGORY_SERVICE_MANAGER
    //!   - CATEGORY_SERVICE_CONTROL
    //!   - CATEGORY_SERVICE
    //! @param lpctszFunctionName   �Ăяo�����֐���(__FUNCTIONW__)
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI TraceFinish(WORD wCategory, LPCTSTR lpctszFunctionName);
    
    //! @brief
    //!   �ėp�g���[�X���O�o�͗p
    //! @param wCategory �ȉ��̒l��ݒ肵�܂��B
    //!   - CATEGORY_SERVICE_CORE
    //!   - CATEGORY_SERVICE_MANAGER
    //!   - CATEGORY_SERVICE_CONTROL
    //!   - CATEGORY_SERVICE
    //! @param lpctszFunctionName   �Ăяo�����֐���(__FUNCTIONW__)
    //! @param lpctszMessage        �ǉ��̏o�̓��b�Z�[�W
    //! @return BOOL
    //! @retval TRUE(����)
    //! @retval FALSE(���s)
    BOOL WINAPI Trace(
        WORD wCategory,
        LPCTSTR lpctszFunctionName,
        LPCTSTR lpctszMessage = nullptr);

private:
    HANDLE _hEventSource;
};
