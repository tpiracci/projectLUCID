// CoordinateMapperHelper.cpp
#include "pch.h"
#include "CoordinateMapperHelper.h"
#include <robuffer.h>
#include <math.h>

using namespace KinectImageProcessor;
using namespace Platform;
using namespace Windows::Storage::Streams;
using namespace WindowsPreview::Kinect;

CoordinateMapperHelper::CoordinateMapperHelper()
{
}

bool CoordinateMapperHelper::ProcessMultiFrameBufferData(
    _In_ const Platform::Array<UINT16>^ depthDataArray,
    _In_ Windows::Storage::Streams::IBuffer^ colorDataBuffer,
    _In_ const Platform::Array<UINT8>^ bodyIndexframeArray,
    _In_ Platform::WriteOnlyArray<UINT8>^ pixelArray)
{
    if (depthDataArray == nullptr || colorDataBuffer == nullptr || bodyIndexframeArray == nullptr || pixelArray == nullptr)
    {
        return false;
    }

    // get active kinect sensor
    KinectSensor^ sensor = WindowsPreview::Kinect::KinectSensor::GetDefault();

    // get coordinatemapper
    CoordinateMapper^ coordinateMapper = sensor->CoordinateMapper;

    int depthWidth = sensor->DepthFrameSource->FrameDescription->Width;
    int depthHeight = sensor->DepthFrameSource->FrameDescription->Height;

    // create color frame description for RGBa format
    FrameDescription^ colorFrameDescription = sensor->ColorFrameSource->CreateFrameDescription(ColorImageFormat::Rgba);

    int colorWidth = colorFrameDescription->Width;
    int colorHeight = colorFrameDescription->Height;

    // assume sensor is already open
    Array<ColorSpacePoint>^ colorPoints = ref new Array<ColorSpacePoint>(depthWidth * depthHeight);
    coordinateMapper->MapDepthFrameToColorSpace(depthDataArray, colorPoints);

    // IBufferByteAccess interface.
    Microsoft::WRL::ComPtr<IBufferByteAccess> bufferByteAccess;
    try
    {
        reinterpret_cast<IInspectable*>(colorDataBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));
    }
    catch (...)
    {
        return false;
    }

    // Retrieve the buffer data.
    byte* pbSource = nullptr;
    bufferByteAccess->Buffer(&pbSource);

    for (int depthIndex = 0; depthIndex < (depthHeight * depthWidth); ++depthIndex)
    {
        byte player = bodyIndexframeArray[depthIndex];

        // if we're tracking a player for the current pixel, sets its color and alpha to full
        if (player != 0xff)
        {
            // retrieve the depth to color mapping for the current depth pixel
            ColorSpacePoint colorPoint = colorPoints[depthIndex];

            // make sure the depth pixel maps to a valid point in color space
            int colorX = (int)floor(colorPoint.X + 0.5);
            int colorY = (int)floor(colorPoint.Y + 0.5);
            if ((colorX >= 0) && (colorX < colorWidth) && (colorY >= 0) && (colorY < colorHeight))
            {
                // calculate index into color array
                UINT8* colorIndex = pbSource + (((colorY * colorWidth) + colorX) * 4);

                // set source for copy to the color pixel
                int displayIndex = depthIndex * 4;

                memcpy((pixelArray->Data)+displayIndex, colorIndex, 4);
            }
        }
    }

    return true;
}

