#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <atlimage.h>
#include <atlhandlerimpl.h>

#define FB3_LOAD_COVERIMAGE 0x02

class FB3Document: public ATL::IDocument
{
public:
    FB3Document(DWORD dwFlags);
    virtual ~FB3Document();

    // Inherited via IDocument
    virtual void BeginReadChunks() override;
    virtual BOOL GetThumbnail(UINT cx, HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha) override;
    virtual void ClearChunkList() override;
    virtual BOOL SetChunkValue(ATL::IFilterChunkValue * value) override;
    virtual BOOL ReadNextChunkValue(ATL::IFilterChunkValue ** value) override;
    virtual void RemoveChunk(REFCLSID guid, DWORD pid) override;
    virtual POSITION FindChunk(REFCLSID guid, DWORD pid) override;
    virtual LPVOID GetContainer() const override;
    virtual HRESULT LoadFromStream(IStream* pStream, DWORD grfMode) override;
    virtual void InitializeSearchContent() override;

private:
    DWORD m_dwFlags;
    ATL::CComPtr<IStream> m_stream{ nullptr };
    ATL::CImage m_image;
    FB3Document() = delete;
};

