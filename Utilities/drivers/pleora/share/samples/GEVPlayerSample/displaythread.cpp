// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************


#include "displaythread.h"
#include "gevplayer.h"
#include "utilities.h"
#include "filteringdlg.h"
#include "imagesavedlg.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QTime>

#include <assert.h>
#include <sstream>

#include <PvFilterRGB.h>


#define TAG_RENDERERMODE ( "renderermode" )


///
/// \brief Constructor
///

DisplayThread::DisplayThread(
    PvDisplayWnd *aDisplayWnd, FilteringDlg *aFilteringDlg,
    ImageSaveDlg* aImageSaveDlg, LogBuffer *aLogBuffer )
    : mDisplayWnd( aDisplayWnd )
    , mImageSaveDlg( aImageSaveDlg )
    , mFilteringDlg( aFilteringDlg )
    , mLogBuffer( aLogBuffer )
{
    mFilteringDlg->SetDisplayThread( this );
    mImageSaveDlg->SetDisplayThread( this );
}


///
/// \brief Destructor
///

DisplayThread::~DisplayThread()
{
    mFilteringDlg->SetDisplayThread( NULL );
    mImageSaveDlg->SetDisplayThread( NULL );
}


///
/// \brief Performs white balance on latest full buffer
///

void DisplayThread::WhiteBalance( PvFilterRGB *aFilterRGB )
{
	PvBuffer *lBuffer = RetrieveLatestBuffer();
    if ( lBuffer == NULL )
    {
        return;
    }

	PvImage *lFinalImage = lBuffer->GetImage();
	assert( lFinalImage != NULL );

    PvBuffer lBufferDisplay;
	PvImage *lImageDisplay = lBufferDisplay.GetImage();
	assert( lImageDisplay != NULL );
    lImageDisplay->Alloc( lFinalImage->GetWidth(), lFinalImage->GetHeight(), PV_PIXEL_WIN_RGB32 );

    // Convert last good buffer to RGB, one-time use converter
    PvBufferConverter lConverter;
    lConverter.Convert( lBuffer, &lBufferDisplay );

    aFilterRGB->WhiteBalance( &lBufferDisplay );

    OnBufferDisplay( lBuffer );

    ReleaseLatestBuffer();
}


///
/// \brief Callback from PvDisplayThread
///

void DisplayThread::OnBufferRetrieved( PvBuffer *aBuffer )
{
    SetBufferLogErrorEnabled( mLogBuffer->IsBufferErrorEnabled() );
    SetBufferLogAllEnabled( mLogBuffer->IsBufferAllEnabled() );
}


///
/// \brief Callback from PvDisplayThread
///

void DisplayThread::OnBufferDisplay( PvBuffer *aBuffer )
{
    // Configure converter, display buffer
    mFilteringDlg->ConfigureConverter( mDisplayWnd->GetConverter() );
    mDisplayWnd->Display( *aBuffer, GetVSyncEnabled() );
}


///
/// \brief Callback from PvDisplayThread
///

void DisplayThread::OnBufferDone( PvBuffer *aBuffer )
{
	// Give the buffer to the image save dialog, it will save it if needed
	mImageSaveDlg->SaveIfNecessary( aBuffer );
}


///
/// \brief Buffer logging callback. Just add to buffer log.
///

void DisplayThread::OnBufferLog( const PvString &aLog )
{
    mLogBuffer->Log( aLog.GetAscii() );
}


///
/// \brief Buffer text overlay callback.
///

void DisplayThread::OnBufferTextOverlay( const PvString &aText )
{
	mDisplayWnd->SetTextOverlay( aText );
}


///
/// \brief Save parameters specific to this display thread.
///

PvResult DisplayThread::Save( PvPropertyList &aPropertyList )
{
	PvResult lResult = PvDisplayThread::Save( aPropertyList );

	PvProperty lProperty;

    // Target FPS
    std::stringstream lTargetFPS;
    lTargetFPS << static_cast<PvUInt32>( mDisplayWnd->GetRendererMode() );
    lProperty.SetName( TAG_RENDERERMODE );
    lProperty.SetValue( lTargetFPS.str().c_str() );
    aPropertyList.Add( lProperty );

	return lResult;
}


///
/// \brief Load parameters specific to this display thread.
///

PvResult DisplayThread::Load( PvPropertyList &aPropertyList )
{
	PvResult lResult = PvDisplayThread::Load( aPropertyList );

	PvInt64 lValueInt;
	PvProperty *lProperty;

    // Target FPS
    lProperty = aPropertyList.GetProperty( TAG_RENDERERMODE );
    if ( lProperty != NULL )
    {
        if ( lProperty->GetValue( lValueInt ).IsOK() )
        {
        	mDisplayWnd->SetRendererMode( static_cast<PvRendererMode>( lValueInt ) );
        }
    }

	return lResult;
}

