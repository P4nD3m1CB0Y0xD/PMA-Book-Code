#pragma once
#include <Windows.h>

#pragma warning( disable : 6387 )


BOOL _IsExplorerProc(DWORD pids);


HMODULE hLib = LoadLibraryA("psapi.dll");
auto ptr_EnumProcessModules = reinterpret_cast<BOOL(WINAPI*)(HANDLE, HMODULE*, DWORD, LPDWORD)>(GetProcAddress(hLib, "EnumProcessModules"));
auto ptr_GetModuleBaseNameA = reinterpret_cast<DWORD(WINAPI*)(HANDLE, HMODULE, LPSTR, DWORD)>(GetProcAddress(hLib, "GetModuleBaseNameA"));
auto ptr_EnumProcesses = reinterpret_cast<BOOL(WINAPI*)(DWORD*, DWORD, LPDWORD)>(GetProcAddress(hLib, "EnumProcesses"));
