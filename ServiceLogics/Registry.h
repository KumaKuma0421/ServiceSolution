//! @file   Registry.h
//! @brief  ���W�X�g���Ɋւ���WindowsAPI���W�񂵂��N���X
//! @author kumakuma0421@gmail.com
//! @date   2021.03.28

#pragma once

#define MAX_KEY_LENGTH 1024

//#include "framework.h"
//#include "WindowsLibrary.h"

enum class RegistryValueTypes
{
    NONE = REG_NONE,
    DWORD = REG_DWORD,
    SZ = REG_SZ,
    EXPAND_SZ = REG_EXPAND_SZ,
    MULTI_SZ = REG_MULTI_SZ,
    BINARY = REG_BINARY,
    QWORD = REG_QWORD,
    QWORD_LITTLE_ENDIAN = REG_QWORD_LITTLE_ENDIAN,
    DWORD_LITTLE_ENDIAN = REG_DWORD_LITTLE_ENDIAN,
    DWORD_BIG_ENDIAN = REG_DWORD_BIG_ENDIAN,
    LINK = REG_LINK,
};

//! @class  Registry
//! @brief ���W�X�g���Ɋւ���WindowsAPI���W�񂵂��N���X
class DECLSPEC Registry
{
public:
    Registry();
    ~Registry();

    //! @brief      ���W�X�g���L�[��V�K�ɍ쐬���܂��B
    //! @param[in]  hRootKey ���W�X�g���̃��[�g�L�[
    //!             HKEY_CLASSES_ROOT
    //!             HKEY_CURRENT_CONFIG
    //!             HKEY_CURRENT_USER
    //!             HKEY_LOCAL_MACHINE
    //!             HKEY_USERS
    //!             HKEY_PERFORMANCE_DATA
    //!             HKEY_PERFORMANCE_TEXT
    //!             HKEY_PERFORMANCE_NLSTEXT
    //!             HKEY_CURRENT_CONFIG
    //!             HKEY_DYN_DATA
    //!             HKEY_CURRENT_USER_LOCAL_SETTINGS
    //! @param[in]  lpctszSubKey �V�K�쐬����T�u�L�[��
    //! @param[in]  bVoratile TRUE�i�������j FALSE�i�s�������j
    //! @return     BOOL
    //! @retval     TRUE(����)
    //! @retval     FALSE(���s)
    BOOL Create(HKEY hRootKey, LPCTSTR lpctszSubKey, BOOL bVoratile = TRUE);

    //! @brief      ���W�X�g���L�[���I�[�v�����܂��B
    //! @param[in]  hRootKey ���W�X�g���̃��[�g�L�[
    //!             HKEY_CLASSES_ROOT
    //!             HKEY_CURRENT_CONFIG
    //!             HKEY_CURRENT_USER
    //!             HKEY_LOCAL_MACHINE
    //!             HKEY_USERS
    //!             HKEY_PERFORMANCE_DATA
    //!             HKEY_PERFORMANCE_TEXT
    //!             HKEY_PERFORMANCE_NLSTEXT
    //!             HKEY_CURRENT_CONFIG
    //!             HKEY_DYN_DATA
    //!             HKEY_CURRENT_USER_LOCAL_SETTINGS
    //! @param[in]  lpctszSubKey �I�[�v������T�u�L�[��
    //! @return     BOOL
    //! @retval     TRUE(����)
    //! @retval     FALSE(���s)
    BOOL Open(HKEY hRootKey, LPCTSTR lpctszSubKey);

    BOOL Query();

    //! @brief      �G���g�����ɐݒ肳�ꂽ�l���擾���܂��B
    //! @param[in]  lpctszEntry �G���g����
    //! @param[in]  eType �G���g���̌^
    //! @param[out] lpbyData �l���擾����o�b�t�@
    //! @param[in]  dwDataLen �l���擾����o�b�t�@�̃T�C�Y
    //! @return     BOOL
    //! @retval     TRUE(����)
    //! @retval     FALSE(���s)
    BOOL QueryValue(
        LPCTSTR lpctszEntry,
        RegistryValueTypes eType,
        LPBYTE lpbyData,
        DWORD dwDataLen);

    //! @brief      �G���g�����ɒl��ݒ肵�܂��B
    //! @param[in]  lpctszEntry �G���g����
    //! @param[in]  eType �G���g���̌^
    //! @param[in]  lpbyData �l��ݒ肷��o�b�t�@
    //! @param[in]  dwDataLen �l��ݒ肷��o�b�t�@�̃T�C�Y
    //! @return     BOOL
    //! @retval     TRUE(����)
    //! @retval     FALSE(���s)
    BOOL SetValue(
        LPCTSTR lpctszEntry,
        RegistryValueTypes eType,
        LPBYTE lpbyData,
        DWORD dwDataLen);

    //! @brief      �T�u�L�[�z�������ׂč폜���܂��B
    //! @param[in]  lpctszSubKey �T�u�L�[��
    //! @return     BOOL
    //! @retval     TRUE(����)
    //! @retval     FALSE(���s)
    BOOL DeleteTree(LPCTSTR lpctszSubKey);

    //! @brief      �T�u�L�[���폜���܂��B
    //! @param[in]  lpctszSubKey �T�u�L�[��
    //! @return     BOOL
    //! @retval     TRUE(����)
    //! @retval     FALSE(���s)
    BOOL DeleteKey(LPCTSTR lpctszSubKey);

    //! @brief      �G���g�����̒l���폜���܂��B
    //! @param[in]  lpctszEntry �G���g����
    //! @return     BOOL
    //! @retval     TRUE(����)
    //! @retval     FALSE(���s)
    BOOL DeleteValue(LPCTSTR lpctszEntry);

    //! @brief      �V�K�쐬/�I�[�v�������T�u�L�[�̐ݒ���t���b�V�����܂��B
    //! @return     BOOL
    //! @retval     TRUE(����)
    //! @retval     FALSE(���s)
    BOOL Flush();

    //! @brief      �V�K�쐬/�I�[�v�������T�u�L�[���N���[�Y���܂��B
    //! @return     BOOL
    //! @retval     TRUE(����)
    //! @retval     FALSE(���s)
    BOOL Close();

    //! @brief      �V�K�쐬/�I�[�v�������T�u�L�[�̃n���h�����擾���܂��B
    //! @return     BOOL
    //! @retval     TRUE(����)
    //! @retval     FALSE(���s)
    HKEY GetKey() { return _hkTarget; };

private:
    HKEY _hkRoot;
    HKEY _hkTarget;
};
