// DepthHelper.cpp
#include "pch.h"
#include "DepthHelper.h"
#include <robuffer.h>

using namespace KinectImageProcessor;
using namespace Platform;
using namespace Windows::Storage::Streams;

DepthHelper::DepthHelper()
{
}

bool DepthHelper::CreateImagePixelDataFromArray(
    _In_ const Platform::Array<UINT16>^ frameArray,
    _In_ Platform::WriteOnlyArray<byte>^ pixelArray)
{
    if (frameArray == nullptr || pixelArray == nullptr)
    {
        return false;
    }

    CopyDepthBytes(reinterpret_cast<byte*>(frameArray->Data), frameArray->Length, pixelArray->Data, pixelArray->Length);

    return true;
}

void DepthHelper::CopyDepthBytes(_In_bytecount_(cbSrcSize) byte* puiSource, int cbSrcSize, _In_bytecount_(cbDestSize) byte* pbDest, int cbDestSize)
{
    UINT16* pSrc = reinterpret_cast<UINT16*>(puiSource);
    RGBQUAD* pRGBX = reinterpret_cast<RGBQUAD*>(pbDest);

    // end pixel is start + width * height - 1
    const UINT16* pBufferEnd = pSrc + cbSrcSize;

    while (pSrc < pBufferEnd)
    {
        UINT16 depth = *pSrc;

        // To convert to a byte, we're discarding the least-significant bits.
        byte intensity = static_cast<byte>(depth);

        pRGBX->rgbRed = intensity;
        pRGBX->rgbGreen = intensity;
        pRGBX->rgbBlue = intensity;
        pRGBX->rgbReserved = 255;

        // adjust pointers
        ++pRGBX;
        ++pSrc;
    }
}