#include "pch.h"
#include <cstdio>

#pragma warning ( disable : 4716 )

[[noreturn]] DWORD WINAPI _PopMsgBox(LPVOID msg_str)
{
    MessageBoxA(nullptr, "Press OK to reboot", reinterpret_cast<LPCSTR>(msg_str), MB_OK);
}

[[noreturn]] DWORD WINAPI _InitActions(LPVOID param)
{
    int i = 0;
    char msg[20];

    for (i; ; ++i)
    {
        sprintf_s(msg, "Thread Action: %d", i);
        CreateThread(
            nullptr,
            0,
            _PopMsgBox,
            msg,
            0,
            0
        );
        Sleep(60000);
    }
}


BOOL APIENTRY DllMain( HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
    DWORD ThreadId;

    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        HANDLE hThread = CreateThread(
            nullptr,
            0,
            _InitActions,
            nullptr,
            0,
            &ThreadId
        );
    }
    return TRUE;
}
