// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include <thread>
#include "resource.h"
#include "../Dwnd/DWnd.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::thread([hModule]() {
            DWnd dwd = DWnd(hModule, IDD_DIALOG1);
            dwd.Run(true);
        }).detach();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

