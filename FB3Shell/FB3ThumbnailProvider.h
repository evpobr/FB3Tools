#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <atlimage.h>
#include <atlhandler.h>
#include <atlhandlerimpl.h>
#include <propsys.h>
#include <thumbcache.h>
#include <wincodec.h>

#include "resource.h"
#include "FB3Shell_i.h"

class ATL_NO_VTABLE FB3ThumbnailProvider :
    public ATL::CComObjectRootEx< ATL::CComSingleThreadModel>,
    public ATL::CComCoClass<FB3ThumbnailProvider, &CLSID_FB3ThumbnailProvider>,
    public ATL::CThumbnailProviderImpl
{
public:
    DECLARE_REGISTRY_RESOURCEID(IDR_FB3THUMBNAILPROVIDER);
    DECLARE_NOT_AGGREGATABLE(FB3ThumbnailProvider)

    BEGIN_COM_MAP(FB3ThumbnailProvider)
        COM_INTERFACE_ENTRY(IInitializeWithStream)
        COM_INTERFACE_ENTRY(IThumbnailProvider)
    END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
        CThumbnailProviderImpl::FinalRelease();
    }

protected:
    ATL::IDocument* CreateDocument();
    virtual HRESULT GetBitmap(_In_ UINT  cx,
                              _In_opt_ HBITMAP *  phbmp,
                              _In_opt_ WTS_ALPHATYPE *  pdwAlpha) override;
};

OBJECT_ENTRY_AUTO(__uuidof(FB3ThumbnailProvider), FB3ThumbnailProvider)
