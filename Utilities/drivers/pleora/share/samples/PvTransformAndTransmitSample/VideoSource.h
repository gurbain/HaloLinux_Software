// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __VIDEO_SOURCE_H__
#define __VIDEO_SOURCE_H__

#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvStream.h>
#include <PvPipeline.h>
#include <PvBufferConverter.h>

#include <QtGui/QImage>

#include <map>


using namespace std;


typedef map<PvUInt32, QImage *> gImageMap;
class PvTransmitterRaw;


// Video source
class VideoSource
{
public:

    VideoSource( const char *aDeviceAddress );
    ~VideoSource();

    PvUInt32 GetWidth();
    PvUInt32 GetHeight();
    PvPixelType GetPixelFormat();

    void Connect();
    void StartAcquisition();
    void StopAcquisition();
    void Disconnect();

    bool FillBuffer( PvBuffer *aBuffer, PvTransmitterRaw *aTransmitter );

private:

    QImage *GetImageForBuffer( PvBuffer *aBuffer );
    void Draw( PvTransmitterRaw *aTransmitter, QImage *aImage );

    std::string mDeviceAddress;

    PvDevice mDevice;
    PvStream mStream;
    PvPipeline *mPipeline;

    PvBufferConverter mConverter;

    gImageMap mMap;
};


#endif // __VIDEO_SOURCE_H__

