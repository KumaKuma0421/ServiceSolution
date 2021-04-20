//
// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "pch.h"

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

