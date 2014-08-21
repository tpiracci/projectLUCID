#pragma once

namespace KinectImageProcessor
{
    public ref class InfraredHelper sealed
    {
    public:
        static bool CreateImagePixelDataFromArray(
            _In_ const Platform::Array<UINT16>^ frameArray,
            _In_ Platform::WriteOnlyArray<byte>^ pixelArray);

    private:
        InfraredHelper();
        static void CopyInfraredBytes(_In_bytecount_(cbSrcSize) byte* puiSource, int cbSrcSize, _In_bytecount_(cbDestSize) byte* pbDest, int cbDestSize);
    };
}