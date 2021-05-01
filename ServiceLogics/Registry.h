//! @file   Registry.h
//! @brief  レジストリに関するWindowsAPIを集約したクラス
//! @author kumakuma0421@gmail.com
//! @date   2021.03.28

#pragma once

//! キー文字列の最大長
#define MAX_KEY_LENGTH 1024

//! @brief レジストリのキー種別
enum class RegistryValueTypes
{
    //! @brief 設定なし
    NONE = REG_NONE,
    //! @brief DWORD型
    DWORD = REG_DWORD,
    //! @brief 文字列型
    SZ = REG_SZ,
    //! @brief %PATH%などを展開して提供する型
    EXPAND_SZ = REG_EXPAND_SZ,
    //! @brief 複数行からなる文字列
    MULTI_SZ = REG_MULTI_SZ,
    //! @brief バイナリ型
    BINARY = REG_BINARY,
    //! @brief 64bit値
    QWORD = REG_QWORD,
    //! @brief 64bit リトルエンディアン値
    QWORD_LITTLE_ENDIAN = REG_QWORD_LITTLE_ENDIAN,
    //! @brief DWORD リトルエンディアン値
    DWORD_LITTLE_ENDIAN = REG_DWORD_LITTLE_ENDIAN,
    //! @brief DWORD ビッグエンディアン型
    DWORD_BIG_ENDIAN = REG_DWORD_BIG_ENDIAN,
    //! @brief リンク型
    LINK = REG_LINK,
};

//! @brief レジストリに関するWindowsAPIを集約したクラス
class DECLSPEC Registry
{
public:
    //! @brief コンストラクタ
    Registry();

    //! @brief デストラクタ
    ~Registry();

    //! @brief
    //!   レジストリキーを新規に作成します。
    //! @param[in]  hRootKey レジストリのルートキー
    //!   - HKEY_CLASSES_ROOT
    //!   - HKEY_CURRENT_CONFIG
    //!   - HKEY_CURRENT_USER
    //!   - HKEY_LOCAL_MACHINE
    //!   - HKEY_USERS
    //!   - HKEY_PERFORMANCE_DATA
    //!   - HKEY_PERFORMANCE_TEXT
    //!   - HKEY_PERFORMANCE_NLSTEXT
    //!   - HKEY_CURRENT_CONFIG
    //!   - HKEY_DYN_DATA
    //!   - HKEY_CURRENT_USER_LOCAL_SETTINGS
    //! @param[in]  lpctszSubKey 新規作成するサブキー名
    //! @param[in]  bVoratile TRUE（揮発性） FALSE（不揮発性）
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL Create(HKEY hRootKey, LPCTSTR lpctszSubKey, BOOL bVoratile = TRUE);

    //! @brief
    //!   レジストリキーをオープンします。
    //! @param[in]  hRootKey レジストリのルートキー
    //!   - HKEY_CLASSES_ROOT
    //!   - HKEY_CURRENT_CONFIG
    //!   - HKEY_CURRENT_USER
    //!   - HKEY_LOCAL_MACHINE
    //!   - HKEY_USERS
    //!   - HKEY_PERFORMANCE_DATA
    //!   - HKEY_PERFORMANCE_TEXT
    //!   - HKEY_PERFORMANCE_NLSTEXT
    //!   - HKEY_CURRENT_CONFIG
    //!   - HKEY_DYN_DATA
    //!   - HKEY_CURRENT_USER_LOCAL_SETTINGS
    //! @param[in]  lpctszSubKey オープンするサブキー名
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL Open(HKEY hRootKey, LPCTSTR lpctszSubKey);

    //! @brief  オープンしているキー配下を列挙します。
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL Query();

    //! @brief  エントリ名に設定された値を取得します。
    //! @param[in]  lpctszEntry エントリ名
    //! @param[in]  eType エントリの型
    //! @param[out] lpbyData 値を取得するバッファ
    //! @param[in]  dwDataLen 値を取得するバッファのサイズ
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL QueryValue(
        LPCTSTR lpctszEntry,
        RegistryValueTypes eType,
        LPBYTE lpbyData,
        DWORD dwDataLen);

    //! @brief  エントリ名に値を設定します。
    //! @param[in]  lpctszEntry エントリ名
    //! @param[in]  eType エントリの型
    //! @param[in]  lpbyData 値を設定するバッファ
    //! @param[in]  dwDataLen 値を設定するバッファのサイズ
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL SetValue(
        LPCTSTR lpctszEntry,
        RegistryValueTypes eType,
        LPBYTE lpbyData,
        DWORD dwDataLen);

    //! @brief  サブキー配下をすべて削除します。
    //! @param[in]  lpctszSubKey サブキー名
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL DeleteTree(LPCTSTR lpctszSubKey);

    //! @brief  サブキーを削除します。
    //! @param[in]  lpctszSubKey サブキー名
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL DeleteKey(LPCTSTR lpctszSubKey);

    //! @brief  エントリ名の値を削除します。
    //! @param[in]  lpctszEntry エントリ名
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL DeleteValue(LPCTSTR lpctszEntry);

    //! @brief  新規作成/オープンしたサブキーの設定をフラッシュします。
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL Flush();

    //! @brief  新規作成/オープンしたサブキーをクローズします。
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    BOOL Close();

    //! @brief  新規作成/オープンしたサブキーのハンドルを取得します。
    //! @return BOOL
    //! @retval TRUE(成功)
    //! @retval FALSE(失敗)
    HKEY GetKey() { return _hkTarget; };

private:
    HKEY _hkRoot;
    HKEY _hkTarget;
};
