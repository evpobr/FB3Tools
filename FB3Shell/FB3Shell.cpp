// FB3Shell.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "resource.h"
#include "FB3Shell_i.h"
#include "dllmain.h"

using namespace ATL;

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}

STDAPI DllRegisterServer(void)
{
    HRESULT hr = _AtlModule.DllRegisterServer();
    return hr;
}

STDAPI DllUnregisterServer(void)
{
    HRESULT hr = _AtlModule.DllUnregisterServer();
    return hr;
}

STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
    HRESULT hr = E_FAIL;
    static const wchar_t szUserSwitch[] = L"user";

    if (pszCmdLine != NULL)
    {
        if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
        {
            ATL::AtlSetPerUserRegistration(true);
        }
    }

    if (bInstall)
    {
        hr = DllRegisterServer();
        if (FAILED(hr))
        {
            DllUnregisterServer();
        }
    }
    else
    {
        hr = DllUnregisterServer();
    }

    return hr;
}
