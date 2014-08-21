#pragma once

namespace KinectImageProcessor
{
    public ref class CoordinateMapperHelper sealed
    {
    public:
        static bool ProcessMultiFrameBufferData(
            _In_ const Platform::Array<UINT16>^ depthDataArray,
            _In_ Windows::Storage::Streams::IBuffer^ colorDataBuffer,
            _In_ const Platform::Array<UINT8>^ bodyIndexframeArray,
            _In_ Platform::WriteOnlyArray<UINT8>^ pixelArray);

    private:
        CoordinateMapperHelper();
    };
}