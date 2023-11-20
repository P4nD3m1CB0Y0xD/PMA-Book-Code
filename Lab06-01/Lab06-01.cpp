#include <Windows.h>
#include <cstdio>
#include <Wininet.h>

#pragma comment(lib, "wininet.lib")
#pragma warning( disable : 6387 )

BOOL CheckInternetAccess(void);

bool main(void)
{
	bool connection_status;

	connection_status = CheckInternetAccess();

	return connection_status;
}


BOOL CheckInternetAccess(void)
{
	BOOL status = FALSE;

	status = InternetGetConnectedState(0, 0);

	if (status == 0)
	{
		printf_s("Error 1.1: No Internet");
	}
	else
	{
		printf_s("Success: Internet Connection");
	}

	return status;
}