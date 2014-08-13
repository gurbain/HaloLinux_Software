// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvBuffer.h>
#include "VideoSource.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
}


///
/// \brief Constructor
///

VideoSource::VideoSource( const std::string &aVideoFile )
    : mVideoFile( aVideoFile )
    , mFormatContext( NULL )
    , mCodecContext( NULL )
    , mSourceFrame( NULL )
    , mCodec( NULL )
    , mVideoIndex( -1 )
{
   // Register all format and codec.
   av_register_all();
}


///
/// \brief Destructor
///

VideoSource::~VideoSource()
{
    // Free the original frame.
    av_free( mSourceFrame );

    // Close the codec.
    avcodec_close( mCodecContext );

    // Close the video file
    avformat_close_input( &mFormatContext );
}


///
/// \brief To find the first video stream.
///

PvInt32 VideoSource::FindFirstVideoStream()
{
    // To holds a single stream (codec).
    AVCodecContext *lCodecCTxt = NULL;

    // Find the first video stream.
    for ( PvInt32 i = 0; i < mFormatContext->nb_streams; i++ )
    {
        lCodecCTxt = mFormatContext->streams[i]->codec;
        switch( lCodecCTxt->codec_type )
        {
            case AVMEDIA_TYPE_VIDEO:
                mVideoIndex = i;
                break;

            default:
                break;
        }
    }

    return mVideoIndex;
}


///
/// \brief To open the video file.
///

void VideoSource::OpenVideoFile()
{
    PvInt32 lNumBytes = 0;
    AVDictionary *lDictionary;

    // Open video file.
    if ( avformat_open_input( &mFormatContext, mVideoFile.c_str(), NULL, NULL ) !=0 )
    {
        cout << "Failed to open video file." << endl;
        exit( 1 );
    }

    // Retrieve stream information.
    if ( avformat_find_stream_info(mFormatContext, NULL) < 0 )
    {
        cout << "Failed to find the stream information." << endl;
        exit( 2 );
    }

    // Get the first video stream
    if ( FindFirstVideoStream() == -1 )
    {
        cout << "Failed to find the video stream." << endl;
        exit( 3 );
    }

    // Get the codec context.
    mCodecContext = mFormatContext->streams[mVideoIndex]->codec;

    // Find the decoder for the video stream.
    mCodec = avcodec_find_decoder( mCodecContext->codec_id );
    if ( mCodec == NULL )
    {
        cout << "Failed to find decoder for the video stream." << endl;
        exit( 4 );
    }

    // Handles the truncated bit stream. Bit stream where frame boundary can fall in the middle of packets.
    if ( mCodec->capabilities & CODEC_CAP_TRUNCATED )
    {
        mCodecContext->flags|=CODEC_FLAG_TRUNCATED;
    }

    // Open codec.
    if ( avcodec_open2( mCodecContext, mCodec, NULL) < 0 )
    {
        cout << "Failed to open codec." << endl;
        exit( 5 );
    }

    // Allocate a video frame to store the original image.
    mSourceFrame = avcodec_alloc_frame();
    if ( mSourceFrame == NULL )
    {
        cout << "Failed to allocate an video frame to store the original image." << endl;
        exit( 6 );
    }
}


///
/// \brief To copy the image extracted from the video file to the PvBuffer aBuffer.
///
/// \param [in] aBuffer - PvBuffer to hold the extracted image from video file.
///

bool VideoSource::CopyImage( PvBuffer *aBuffer )
{
    PvInt32 lFrameDecoded = 0;
    PvInt32 lNumBytes = 0;
    AVPacket lPacket;
    SwsContext *lSWSContext = NULL;
    AVFrame lDestFrame;

    if ( av_read_frame( mFormatContext, &lPacket ) >= 0 )
    {
      // Check the packet to make make sure it is from the video stream?
      if ( lPacket.stream_index == mVideoIndex )
      {
        // Decode video frame
        avcodec_decode_video2( mCodecContext, mSourceFrame, &lFrameDecoded, &lPacket );

        if ( lFrameDecoded )
        {
			// Assign appropriate parts of buffer to the frame.
			avpicture_fill( (AVPicture*)&lDestFrame, aBuffer->GetDataPointer(), PIX_FMT_BGR24, mCodecContext->width, mCodecContext->height );
        
            // Get the SwsContext to perform image scaling/conversion.
            lSWSContext = sws_getContext( mCodecContext->width, mCodecContext->height, 
                mCodecContext->pix_fmt, mCodecContext->width, mCodecContext->height,  
                PIX_FMT_BGR24, SWS_BILINEAR, 0, 0, 0 );
            if (lSWSContext != NULL)
            {
                // Convert the video frame from its native format to BGR 24 bits.
                sws_scale( lSWSContext, mSourceFrame->data, mSourceFrame->linesize, 0, 
                    mCodecContext->height, lDestFrame.data, lDestFrame.linesize );
            }
            else
            {
                cout << "Failed to convert a video frame from its native format to BGR 24 bits." << endl;
            }

            // Free the SwsContext that was allocated by sws_getContext.
			av_free( lSWSContext );

			// Free the packet that was allocated by av_read_frame.
			av_free_packet( &lPacket );
        }
      }
    }
    else
    {
		// End of video was reached: re-open the video file.
		avformat_close_input( &mFormatContext );
        OpenVideoFile();
    }

    return ( lFrameDecoded != 0 );
}


///
/// \brief Get width of the image created from the video stream.
///

PvUInt32 VideoSource::GetWidth()
{
    return  mCodecContext->width;
}


///
/// \brief Get height of the image created from the video stream.
///

PvUInt32 VideoSource::GetHeight()
{
    return  mCodecContext->height;
}



