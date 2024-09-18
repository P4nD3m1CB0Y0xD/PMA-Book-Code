#include <iostream>
#include <Windows.h>

#pragma comment(lib, "Urlmon.lib")

class HttpRequest
{
public:
    void _DownloadFile(LPCSTR url)
    {
        URLDownloadToFileA(nullptr, url, "C:\\tempdownload.exe", 0, 0);
    }
};

int main(void)
{
    HttpRequest resquest;
    resquest._DownloadFile("http://127.0.0.1:8000/cpp.html");
    return 0;
}
