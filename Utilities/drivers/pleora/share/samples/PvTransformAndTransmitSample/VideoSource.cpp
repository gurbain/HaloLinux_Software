// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvDeviceFinderWnd.h>
#include <PvTransmitterRaw.h>
#include <PvVersion.h>

#include <sstream>

#include <QtGui/QPainter>

#include "VideoSource.h"


VideoSource::VideoSource( const char *aDeviceAddress )
    : mDeviceAddress( aDeviceAddress )
{
    mPipeline = new PvPipeline( &mStream );
}

VideoSource::~VideoSource()
{
    // Pipeline was dynamically allcated, release it
    if ( mPipeline != NULL )
    {
        delete mPipeline;
        mPipeline = NULL;
    }

    // Free images
	gImageMap::iterator lIt = mMap.begin();
    while ( lIt != mMap.end() )
    {
        QImage *lImage = ( lIt++ )->second;
        delete lImage;
		lImage = NULL;
    }
}

void VideoSource::Connect()
{
    if ( mDeviceAddress.size() <= 0 )
    {
        // No device address, prompts user
        PvDeviceFinderWnd lWnd;
        if ( lWnd.ShowModal().IsOK() )
        {
            mDeviceAddress = lWnd.GetSelected()->GetIPAddress().GetAscii();
        }
    }

    // Connect device
    PvResult lResult = mDevice.Connect( mDeviceAddress.c_str() );
    if ( !lResult )
    {
        cout << "Unable to connect to device." << endl;
        exit( 1 );
    }

    // Open stream
    lResult = mStream.Open( mDeviceAddress.c_str() );
    if ( !lResult )
    {
        cout << "Unable to open stream." << endl;
        exit( 1 );
    }
}

void VideoSource::StartAcquisition()
{
    // Start pipeline
    mPipeline->Start();

    // Set streaming destination
    mDevice.SetStreamDestination( mStream.GetLocalIPAddress(), mStream.GetLocalPort() );

    // Set TLParamsLocked to 1
    mDevice.GetGenParameters()->SetIntegerValue( "TLParamsLocked", 1 );

    // Start acquisition
    mDevice.GetGenParameters()->ExecuteCommand( "AcquisitionStart" );
}

void VideoSource::StopAcquisition()
{
    // Stop acquisition
    mDevice.GetGenParameters()->ExecuteCommand( "AcquisitionStop" );

    // Set TLParamsLocked to 0
    mDevice.GetGenParameters()->SetIntegerValue( "TLParamsLocked", 0 );

    // Stop pipeline
    mPipeline->Stop();
}

void VideoSource::Disconnect()
{
    if ( mStream.IsOpen() )
    {
        // Close stream
        mStream.Close();
    }

    if ( mDevice.IsConnected() )
    {
        // Disconect device
        mDevice.Disconnect();
    }
}

bool VideoSource::FillBuffer( PvBuffer *aBuffer, PvTransmitterRaw *aTransmitter )
{
    // Get input buffer from pipeline (stream in)
    PvBuffer *lInBuffer = NULL;
    if ( !mPipeline->RetrieveNextBuffer( &lInBuffer, 100 ).IsOK() )
    {
        return false;
    }

    // We got a buffer, but is it good?
    if ( lInBuffer->GetOperationResult().IsOK() )
    {
		// Get image by ID (one for every buffer, attached for the buffer)
		QImage *lImage = GetImageForBuffer( aBuffer );

		PvUInt32 lWidth = GetWidth();
		PvUInt32 lHeight = GetHeight();
		PvUInt32 lPaddingX = lImage->bytesPerLine() - ( lWidth * 3 );

		// Attach image to output buffer with conversion-happy pixel type
		aBuffer->GetImage()->Attach( lImage->bits(),
			lWidth, lHeight, PvPixelRGB8, lPaddingX );

		// Copy in buffer into image attached to out buffer using eBUS SDK pixel converters
		mConverter.Convert( lInBuffer, aBuffer, false );

		// Draw (transform part of the sample) on image attached to out buffer
		Draw( aTransmitter, lImage );
    }

	// Release buffer back to pipeline
	mPipeline->ReleaseBuffer( lInBuffer );

	return lInBuffer->GetOperationResult().IsOK();
}

PvUInt32 VideoSource::GetWidth()
{
    PvInt64 lWidth;
    mDevice.GetGenParameters()->GetIntegerValue( "Width", lWidth );

    return static_cast<PvUInt32>( lWidth );
}

PvUInt32 VideoSource::GetHeight()
{
    PvInt64 lHeight;
    mDevice.GetGenParameters()->GetIntegerValue( "Height", lHeight );

    return static_cast<PvUInt32>( lHeight );
}

PvPixelType VideoSource::GetPixelFormat()
{
    PvInt64 lFormat;
    mDevice.GetGenParameters()->GetEnumValue( "PixelFormat", lFormat );

    return static_cast<PvPixelType>( lFormat );
}

QImage *VideoSource::GetImageForBuffer( PvBuffer *aBuffer )
{
	static int sNextID = 1;
    PvUInt32 lID = static_cast<PvUInt32>( aBuffer->GetID() );
    if ( lID == 0 )
    {
    	// Make sure all buffers have unique IDs
    	aBuffer->SetID( sNextID++ );
    }

    QImage *lImage = NULL;

    PvUInt32 lWidth = GetWidth();
    PvUInt32 lHeight = GetHeight();

    if ( mMap.find( lID ) == mMap.end() )
    {
        // No buffer found at map position
    }
    else
    {
        lImage = mMap[ lID ];
        if ( ( lImage->size().width() != lWidth ) ||
             ( lImage->size().height() != lHeight ) )
        {
            // Detach from PvBuffer
            aBuffer->Detach();

            // Image found, but not the right size
            delete lImage;
            lImage = NULL;
        }
    }

    if ( lImage == NULL )
    {
        // Create a new image of the right size
        lImage = new QImage( lWidth, lHeight, QImage::Format_RGB888 );
        mMap[ lID ] = lImage;
    }

    return lImage;
}

void VideoSource::Draw( PvTransmitterRaw *aTransmitter, QImage *aImage )
{
    stringstream lSS;

    lSS << "PvTransformAndTransmitSample" << endl;
    lSS << PRODUCT_NAME << " " << NVERSION_STRING << endl << endl;

    lSS << fixed << setprecision( 1 );
    lSS << "Transmitted " << aTransmitter->GetBlocksTransmitted() << " blocks" << endl;

    lSS << fixed << setprecision( 1 );
    lSS << aTransmitter->GetAverageTransmissionRate() << " (" << aTransmitter->GetInstantaneousTransmissionRate() << ") FPS" << endl;

    lSS << fixed << setprecision( 1 );
    lSS << aTransmitter->GetAveragePayloadThroughput() / 1000000.0f <<
        " (" << aTransmitter->GetInstantaneousPayloadThroughput() / 1000000.0f << ") Mb/s" << endl;

    QRect lRectangle( QPoint( 25, 25 ) , QPoint( aImage->width(), aImage->height() ) );

    QFont lFont( "Arial", 16 );

    QPainter lPainter( aImage );
	lPainter.setPen( Qt::cyan );
	lPainter.setFont( lFont );

	lPainter.drawText( lRectangle, Qt::AlignTop | Qt::AlignLeft, lSS.str().c_str() );
}


