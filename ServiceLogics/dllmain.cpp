//! @file   dllmain.cpp
//! @brief	DLL アプリケーションのエントリ ポイントを定義します。
//! @author	kumakuma0421@gmail.com
//! @date	2021.4.29

#include "pch.h"

//! @brief DLLエントリーポイント
//! @param hModule DLLモジュールへのハンドル
//! @param dwReasonForCall DLLエントリポイント関数が呼び出される理由
//!   - DLL_PROCESS_ATTACH
//!   - DLL_THREAD_ATTACH
//!   - DLL_THREAD_DETACH
//!   - DLL_PROCESS_DETACH
//! @param lpvReserved リザーブ
//! @return TRUE
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  dwReasonForCall,
                      LPVOID lpvReserved
)
{
    switch (dwReasonForCall)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
