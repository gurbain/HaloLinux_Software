// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __VIDEO_SOURCE_H__
#define __VIDEO_SOURCE_H__

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVCodec;

class PvBuffer;

// Video source
class VideoSource
{
public:

    VideoSource( const std::string &aVideoFile );

    ~VideoSource();

    void OpenVideoFile();

    bool CopyImage( PvBuffer *aBuffer );

    PvUInt32 GetWidth();

    PvUInt32 GetHeight();

private:

    PvInt32 FindFirstVideoStream();

    AVFormatContext *mFormatContext;
    AVCodecContext  *mCodecContext;
    AVFrame *mSourceFrame;
    AVCodec *mCodec;

    const std::string mVideoFile;

    PvInt32 mVideoIndex;
};


#endif // __VIDEO_SOURCE_H__
