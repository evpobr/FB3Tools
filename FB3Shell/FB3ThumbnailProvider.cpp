#include "stdafx.h"

#include "FB3Shell_i.h"
#include "FB3Document.h"
#include "FB3ThumbnailProvider.h"

using namespace ATL;

IDocument * FB3ThumbnailProvider::CreateDocument()
{
    FB3Document *pDocument = NULL;
    ATLTRY(pDocument = new FB3Document(FB3_LOAD_COVERIMAGE));
    return pDocument;
}

HRESULT FB3ThumbnailProvider::GetBitmap(UINT cx, HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha)
{
    if (m_pDocument == NULL)
    {
        return E_NOTIMPL;
    }

    if (!m_pDocument->GetThumbnail(cx, phbmp, pdwAlpha))
    {
        return E_FAIL;
    }

    return S_OK;
}
