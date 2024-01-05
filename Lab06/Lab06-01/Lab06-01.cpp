#include <Windows.h>
#include <cstdio>
#include <Wininet.h>

#pragma comment(lib, "wininet.lib")
#pragma warning( disable : 6387 )

BOOL CheckInternetAccess(void);

int main(int argc, char* argv, char* envp)
{
	int state;

	if (CheckInternetAccess() != 0)
		state = TRUE;
	else
		state = FALSE;

	return state;
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