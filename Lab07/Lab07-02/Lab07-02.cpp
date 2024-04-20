#include <Windows.h>
#include <stdio.h>
#include <exdisp.h>

int main(void)
{
    VARIANT pvarg = { 0 };
    BSTR str_url;
    IWebBrowser2* pBrowser = nullptr;

    if (OleInitialize(NULL) >= S_OK)
    {
        // CLSID_InternetExplorer = 0002DF01-0000-0000-C000-000000000046
        // IID_IWebBrowser2       = D30C1661-CDAF-11D0-8A3E-00C04FC9E26E
        HRESULT hr = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pBrowser);
        if (pBrowser)
        {
            VariantInit(&pvarg);
            str_url = SysAllocString(L"http://192.0.0.128:8000/hello.html");
            /* pvarg.vt & pvarg.lVal specify that the navigation should open in a new window. */
            pvarg.vt = 3;
            pvarg.lVal = 1; 
            pBrowser->Navigate(str_url, &pvarg, &pvarg, &pvarg, &pvarg);
            SysFreeString(str_url);
        }
    }
    OleUninitialize();

    return 0;
}

