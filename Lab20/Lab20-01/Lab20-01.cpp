#include <Windows.h>
#include <urlmon.h>

#pragma comment(lib, "Urlmon.lib")

class HttpRequest
{
public:
	const char* url;
	void DownloadFile()
	{
		::URLDownloadToFileA(
			nullptr,
			url,
			"C:\\Users\\Public\\tempdownload.exe", // <-- New Windows versions doesn't allow an unprivileged user to write to the C:\ directory. So I use the PUBLIC just as an example.
			0,
			nullptr
		);
	}
};

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow
)
{
	HttpRequest* request = new HttpRequest;
	request->url = "http://127.0.0.1:8000/cpp.html"; // <-- Open a CMD and run: "python3 -m http.server -b 127.0.0.1"
	request->DownloadFile();
	// delete request; // <-- Best practice. But, in the original binary, it doesn't use the delete operator.
	return 0;
}
