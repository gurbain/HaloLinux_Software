// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <PvBuffer.h>
#include <PvBufferConverter.h>
#include <PvFilterDeInterlace.h>
#include <PvFPSStabilizer.h>
#include <PvDisplayWnd.h>
#include <PvDisplayThread.h>

#include "logbuffer.h"


class ImageSaveDlg;
class FilteringDlg;




class DisplayThread : public PvDisplayThread
{
public:

	DisplayThread(
        PvDisplayWnd *aDisplayWnd, FilteringDlg *aFilteringDlg,
        ImageSaveDlg* aImageSaveDlg, LogBuffer *aLogBuffer );
	~DisplayThread();

    void WhiteBalance( PvFilterRGB *aFilterRGB );

    PvRendererMode GetRendererMode() const { return mDisplayWnd->GetRendererMode(); }
    void SetRendererMode( PvRendererMode aMode ) const { mDisplayWnd->SetRendererMode( aMode ); }

    virtual PvResult Save( PvPropertyList &aPropertyList );
    virtual PvResult Load( PvPropertyList &aPropertyList );

protected:

    void OnBufferRetrieved( PvBuffer *aBuffer );
    void OnBufferDisplay( PvBuffer *aBuffer );
    void OnBufferDone( PvBuffer *aBuffer );
    void OnBufferLog( const PvString &aLog );
    void OnBufferTextOverlay( const PvString &aText );

private:

	PvDisplayWnd *mDisplayWnd;

    FilteringDlg *mFilteringDlg;
    ImageSaveDlg *mImageSaveDlg;
	LogBuffer *mLogBuffer;
};

