// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include <thread>

#ifdef DEBUG
#elif R
#endif // DEBUG


INT_PTR ApplicationStart(HMODULE hModule);

HMODULE hInstance;

INT_PTR Run() {
    return ApplicationStart(hInstance);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hInstance = hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

