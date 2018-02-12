#include "stdafx.h"
#include "FB3Document.h"

using namespace ATL;

FB3Document::FB3Document(DWORD dwFlags)
{
    m_dwFlags = dwFlags;
}


FB3Document::~FB3Document()
{
}

HRESULT FB3Document::LoadFromStream(IStream * pStream, DWORD grfMode)
{
    CString strThumbnailRelationshipType = L"http://schemas.openxmlformats.org/package/2006/relationships/metadata/thumbnail";

    UNREFERENCED_PARAMETER(grfMode);

    if (!pStream)
        return E_INVALIDARG;

    if (m_stream)
        return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);

    HRESULT hr = pStream->QueryInterface(IID_PPV_ARGS(&m_stream));
    if (SUCCEEDED(hr))
    {
        CComPtr<IOpcFactory> factory;
        hr = factory.CoCreateInstance(__uuidof(OpcFactory));
        if (SUCCEEDED(hr))
        {
            CComPtr<IOpcPackage> package;
            hr = factory->ReadPackageFromStream(m_stream, OPC_READ_DEFAULT, &package);
            if (SUCCEEDED(hr))
            {
                CComPtr<IOpcRelationshipSet> relationshipSet;
                hr = package->GetRelationshipSet(&relationshipSet);
                BOOL found = FALSE;
                if (SUCCEEDED(hr))
                {
                    CComPtr<IOpcRelationshipEnumerator> relationshipEnumerator;
                    hr = relationshipSet->GetEnumerator(&relationshipEnumerator);
                    if (SUCCEEDED(hr))
                    {
                        BOOL hasNext = TRUE;
                        while (!((hasNext == FALSE) || (found == TRUE)))
                        {
                            hr = relationshipEnumerator->MoveNext(&hasNext);
                            if (SUCCEEDED(hr) && (hasNext == TRUE))
                            {
                                CComPtr<IOpcRelationship> relationship;
                                hr = relationshipEnumerator->GetCurrent(&relationship);
                                if (SUCCEEDED(hr))
                                {
                                    CComHeapPtr<WCHAR> relationshipType;
                                    hr = relationship->GetRelationshipType(&relationshipType);
                                    if (SUCCEEDED(hr))
                                    {
                                        if (m_dwFlags & FB3_LOAD_COVERIMAGE)
                                        {
                                            if (strThumbnailRelationshipType == relationshipType)
                                            {
                                                CComPtr<IOpcUri> sourceUri;
                                                hr = relationship->GetSourceUri(&sourceUri);
                                                if (SUCCEEDED(hr))
                                                {
                                                    CComPtr<IUri> targetUri;
                                                    hr = relationship->GetTargetUri(&targetUri);
                                                    if (SUCCEEDED(hr))
                                                    {
                                                        CComPtr<IOpcPartUri> partUri;
                                                        hr = sourceUri->CombinePartUri(targetUri, &partUri);
                                                        if (SUCCEEDED(hr))
                                                        {
                                                            CComPtr<IOpcPartSet> partSet;

                                                            hr = package->GetPartSet(&partSet);
                                                            if (SUCCEEDED(hr))
                                                            {
                                                                BOOL fExists = FALSE;
                                                                partSet->PartExists(partUri, &fExists);
                                                                if (SUCCEEDED(hr) && (fExists == TRUE))
                                                                {
                                                                    CComPtr<IOpcPart> part;
                                                                    hr = partSet->GetPart(partUri, &part);
                                                                    if (SUCCEEDED(hr))
                                                                    {
                                                                        CComPtr<IStream> imageStream;
                                                                        hr = part->GetContentStream(&imageStream);
                                                                        if (SUCCEEDED(hr))
                                                                        {
                                                                            hr = m_image.Load(imageStream);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            found = TRUE;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        relationship.Release();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (FAILED(hr))
    {
        m_stream.Release();
    }

    return hr;
}

void FB3Document::InitializeSearchContent()
{
}

void FB3Document::BeginReadChunks()
{
}

BOOL FB3Document::GetThumbnail(UINT cx, HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha)
{
    if (cx == 0)
        return FALSE;
    if (!phbmp || !pdwAlpha)
        return FALSE;
    if (!m_image)
        return FALSE;

    *phbmp = nullptr;
    *pdwAlpha = WTSAT_UNKNOWN;

    BOOL fRet = FALSE;

    int width, height;
    float scale = 0.0f;
    width = m_image.GetWidth();
    height = m_image.GetHeight();
    if ((width > 0) && (height > 0))
    {
        int thumbWidth = width;
        int thumbHeight = height;
        if ((width > static_cast<int>(cx)) || (height > static_cast<int>(cx)))
        {
            if (width <= height)
            {
                scale = static_cast<float>(cx) / static_cast<float>(height);
            }
            else
            {
                scale = static_cast<float>(cx) / static_cast<float>(width);
            }
            thumbWidth = static_cast<int>(static_cast<float>(width) * scale);
            thumbHeight = static_cast<int>(static_cast<float>(height) * scale);
        }

        CImage thumb;
        fRet = thumb.Create(thumbWidth, thumbHeight, 32);
        if (fRet)
        {
            fRet = m_image.Draw(
                thumb.GetDC(),
                CRect(0, 0, thumbWidth, thumbHeight),
                Gdiplus::InterpolationMode::InterpolationModeHighQuality);

            if (fRet)
            {
                thumb.ReleaseDC();
                *phbmp = thumb.Detach();
                *pdwAlpha = WTSAT_UNKNOWN;
                fRet = TRUE;
            }
        }
    }

    return fRet;
}

void FB3Document::ClearChunkList()
{
}

BOOL FB3Document::SetChunkValue(IFilterChunkValue * value)
{
    return FALSE;
}

BOOL FB3Document::ReadNextChunkValue(IFilterChunkValue ** value)
{
    return FALSE;
}

void FB3Document::RemoveChunk(REFCLSID guid, DWORD pid)
{
}

POSITION FB3Document::FindChunk(REFCLSID guid, DWORD pid)
{
    return nullptr;
}

LPVOID FB3Document::GetContainer() const
{
    return (LPVOID)this;
}
