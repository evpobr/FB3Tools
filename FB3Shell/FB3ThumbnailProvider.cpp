#include "stdafx.h"

using namespace Microsoft::WRL;

LPCWSTR g_pszThumbnailRelationshipType = L"http://schemas.openxmlformats.org/package/2006/relationships/metadata/thumbnail";

// {0C1CF396-AF95-497D-AED7-AD1D73A1C550}
DEFINE_GUID(CLSID_FB3ThumbnailProvider,
            0xc1cf396, 0xaf95, 0x497d, 0xae, 0xd7, 0xad, 0x1d, 0x73, 0xa1, 0xc5, 0x50);

class
    DECLSPEC_UUID("{0C1CF396-AF95-497D-AED7-AD1D73A1C550}")
    FB3ThumbnailProvider : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IInitializeWithStream, IThumbnailProvider>
{
public:
    // Inherited via IInitializeWithStream
    IFACEMETHODIMP Initialize(IStream * pstream, DWORD grfMode)
    {
        UNREFERENCED_PARAMETER(grfMode);

        if (!pstream)
            return E_INVALIDARG;

        if (m_stream)
            return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);

        HRESULT hr = pstream->QueryInterface(IID_PPV_ARGS(&m_stream));
        if (SUCCEEDED(hr))
        {
            ComPtr<IOpcFactory> factory;
            hr = CoCreateInstance(__uuidof(OpcFactory), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IOpcFactory),
                                  reinterpret_cast<void **>(factory.GetAddressOf()));
            if (SUCCEEDED(hr))
            {
                ComPtr<IOpcPackage> package;
                hr = factory->ReadPackageFromStream(m_stream.Get(), OPC_READ_DEFAULT, package.GetAddressOf());
                if (SUCCEEDED(hr))
                {
                    ComPtr<IOpcRelationshipSet> relationshipSet;
                    hr = package->GetRelationshipSet(relationshipSet.GetAddressOf());
                    BOOL found = FALSE;
                    if (SUCCEEDED(hr))
                    {
                        ComPtr<IOpcRelationshipEnumerator> relationshipEnumerator;
                        hr = relationshipSet->GetEnumerator(relationshipEnumerator.GetAddressOf());
                        if (SUCCEEDED(hr))
                        {
                            BOOL hasNext = TRUE;
                            while (!((hasNext == FALSE) || (found == TRUE)))
                            {
                                hr = relationshipEnumerator->MoveNext(&hasNext);
                                if (SUCCEEDED(hr) && (hasNext == TRUE))
                                {
                                    ComPtr<IOpcRelationship> relationship;
                                    hr = relationshipEnumerator->GetCurrent(relationship.GetAddressOf());
                                    if (SUCCEEDED(hr))
                                    {
                                        LPWSTR relationshipType;
                                        hr = relationship->GetRelationshipType(&relationshipType);
                                        if (SUCCEEDED(hr))
                                        {
                                            if (lstrcmpiW(relationshipType, g_pszThumbnailRelationshipType) == 0)
                                            {
                                                ComPtr<IOpcUri> sourceUri;
                                                hr = relationship->GetSourceUri(sourceUri.GetAddressOf());
                                                if (SUCCEEDED(hr))
                                                {
                                                    ComPtr<IUri> targetUri;
                                                    hr = relationship->GetTargetUri(targetUri.GetAddressOf());
                                                    if (SUCCEEDED(hr))
                                                    {
                                                        ComPtr<IOpcPartUri> partUri;
                                                        hr = sourceUri->CombinePartUri(targetUri.Get(), partUri.GetAddressOf());
                                                        if (SUCCEEDED(hr))
                                                        {
                                                            ComPtr<IOpcPartSet> partSet;

                                                            hr = package->GetPartSet(partSet.GetAddressOf());
                                                            if (SUCCEEDED(hr))
                                                            {
                                                                BOOL fExists = FALSE;
                                                                partSet->PartExists(partUri.Get(), &fExists);
                                                                if (SUCCEEDED(hr) && (fExists == TRUE))
                                                                {
                                                                    ComPtr<IOpcPart> part;
                                                                    hr = partSet->GetPart(partUri.Get(), part.GetAddressOf());
                                                                    if (SUCCEEDED(hr))
                                                                    {
                                                                        ComPtr<IStream> imageStream;
                                                                        hr = part->GetContentStream(imageStream.GetAddressOf());
                                                                        if (SUCCEEDED(hr))
                                                                        {
                                                                            hr = LoadImageFromStream(imageStream.Get(), m_image.GetAddressOf());
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            found = TRUE;
                                                        }
                                                    }
                                                }
                                            }
                                            CoTaskMemFree(relationshipType);
                                            relationship.Reset();
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
            m_stream.Reset();
        }

        return hr;
    }

    // Inherited via IThumbnailProvider
    IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha)
    {
        if (cx == 0)
            return E_INVALIDARG;
        if (!phbmp || !pdwAlpha)
            return E_POINTER;
        if (!m_image)
            return E_FAIL;

        *phbmp = nullptr;
        *pdwAlpha = WTSAT_UNKNOWN;

        UINT width, height;
        float scale = 0.0f;
        HRESULT hr = m_image->GetSize(&width, &height);
        if (SUCCEEDED(hr))
        {
            UINT thumbWidth = width;
            UINT thumbHeight = height;
            if ((width > cx) || (height > cx))
            {
                if (width <= height)
                {
                    scale = (float)cx / (float)height;
                }
                else
                {
                    scale = (float)cx / (float)width;
                }
                thumbWidth = static_cast<UINT>(static_cast<float>(width) * scale);
                thumbHeight = static_cast<UINT>(static_cast<float>(height) * scale);
            }

            ComPtr<IWICImagingFactory> factory;
            hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory),
                                  reinterpret_cast<void **>(factory.GetAddressOf()));
            if (SUCCEEDED(hr))
            {
                ComPtr<IWICBitmapScaler> scaler;
                hr = factory->CreateBitmapScaler(&scaler);
                if (SUCCEEDED(hr))
                {
                    hr = scaler->Initialize(m_image.Get(), thumbWidth, thumbHeight, WICBitmapInterpolationModeHighQualityCubic);
                    if (SUCCEEDED(hr))
                    {
                        ComPtr<IWICBitmapSource> scaledBitmap;
                        hr = scaler.As(&scaledBitmap);
                        if (SUCCEEDED(hr))
                        {
                            hr = WICBitmapToHBITMAP(scaledBitmap.Get(), phbmp);
                        }
                    }
                }
            }
        }

        return hr;
    }

private:
    ComPtr<IStream> m_stream{ nullptr };
    ComPtr<IWICBitmapSource> m_image;

    HRESULT LoadImageFromStream(IStream * pstream, IWICBitmapSource ** ppBitmap)
    {
        if (!pstream)
            return E_INVALIDARG;
        if (!ppBitmap)
            return E_POINTER;

        *ppBitmap = nullptr;

        ComPtr<IWICImagingFactory> factory;
        HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory),
                                      reinterpret_cast<void **>(factory.GetAddressOf()));
        if (SUCCEEDED(hr))
        {
            ComPtr<IWICBitmapDecoder> decoder;
            hr = factory->CreateDecoderFromStream(pstream, nullptr, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                UINT framesCount;
                hr = decoder->GetFrameCount(&framesCount);
                if (SUCCEEDED(hr))
                {
                    ComPtr<IWICBitmapFrameDecode> frameDecode;
                    hr = decoder->GetFrame(0, frameDecode.GetAddressOf());
                    if (SUCCEEDED(hr))
                    {
                        WICPixelFormatGUID pixelFormat;
                        hr = frameDecode->GetPixelFormat(&pixelFormat);
                        if (SUCCEEDED(hr))
                        {
                            if (pixelFormat != GUID_WICPixelFormat32bppBGRA)
                            {
                                ComPtr<IWICBitmapSource> source, dest;
                                source = frameDecode;
                                if (SUCCEEDED(hr))
                                {
                                    hr = WICConvertBitmapSource(GUID_WICPixelFormat32bppBGRA, source.Get(), dest.GetAddressOf());
                                    if (SUCCEEDED(hr))
                                    {
                                        source.Reset();
                                        source = dest;
                                        *ppBitmap = source.Detach();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return hr;
    }

    HRESULT WICBitmapToHBITMAP(IWICBitmapSource * pBitmap, HBITMAP * phbmp)
    {
        if (!pBitmap)
            return E_INVALIDARG;
        if (!phbmp)
            return E_POINTER;

        *phbmp = nullptr;

        HBITMAP hbmp = nullptr;

        UINT width = 0;
        UINT height = 0;

        HRESULT hr = pBitmap->GetSize(&width, &height);
        if (SUCCEEDED(hr) && (width != 0) && (height != 0))
        {
            BITMAPINFO bminfo = { 0 };
            bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bminfo.bmiHeader.biWidth = width;
            bminfo.bmiHeader.biHeight = -((LONG)height);
            bminfo.bmiHeader.biPlanes = 1;
            bminfo.bmiHeader.biBitCount = 32;
            bminfo.bmiHeader.biCompression = BI_RGB;

            DWORD dwError;
            void * pvImageBits = nullptr;
            HDC hdcScreen = GetDC(nullptr);
            if (!hdcScreen)
            {
                dwError = GetLastError();
                hr = HRESULT_FROM_WIN32(dwError);
            }
            else
            {
                hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, nullptr, 0);
                if (!hbmp)
                {
                    dwError = GetLastError();
                    hr = HRESULT_FROM_WIN32(dwError);
                }
                ReleaseDC(nullptr, hdcScreen);

                if (hbmp)
                {
                    const UINT cbStride = width * 4;
                    const UINT cbImage = cbStride * height;

                    hr = pBitmap->CopyPixels(nullptr, cbStride, cbImage, static_cast<BYTE *>(pvImageBits));
                    if (SUCCEEDED(hr))
                    {
                        *phbmp = hbmp;
                    }
                    else
                    {
                        DeleteObject(hbmp);
                    }

                }
            }
        }
        return hr;
    }
};

CoCreatableClass(FB3ThumbnailProvider);
