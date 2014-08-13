// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// To receive images using PvPipeline
//

#include <PvSampleUtils.h>
#include <PvStream.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#ifdef PV_GUI_NOT_AVAILABLE
#include <PvSystem.h>
#else
#include <PvDeviceFinderWnd.h>
#endif // PV_GUI_NOT_AVAILABLE

PV_INIT_SIGNAL_HANDLER();

//
// Select the device we will be receiving data from. 
//
// We only return the IP address (which we need) and WILL NOT 
// connect to the device. We will just passively open a multicast
// stream as a receiver from this device.
//

bool SelectDevice( PvString &aIP )
{
    PvDeviceInfo *lDeviceInfo = NULL;
    PvResult lResult;

#ifdef PV_GUI_NOT_AVAILABLE
	PvSystem lSystem;
    lDeviceInfo = PvSelectDevice( lSystem );
    if( lDeviceInfo == NULL )
    {
        cout << "No device selected." << endl;
        return false;
    }
#else
	PvDeviceFinderWnd lDeviceFinderWnd;
    lResult = lDeviceFinderWnd.ShowModal();
    if( !lResult.IsOK() )
    {
        cout << "No device selected." << endl;
        return false;
    }
    lDeviceInfo = lDeviceFinderWnd.GetSelected();
#endif // PV_GUI_NOT_AVAILABLE
	
	aIP = lDeviceInfo->GetIPAddress();

	return true;
}

//
// PvPipeline event handler class. Used to trap buffer too small events
// and let the pipeline know we want all the buffers to be resized immediately.
//

class PipelineEventSink : public PvPipelineEventSink
{
protected:

    void OnBufferTooSmall( PvPipeline *aPipeline, bool *aReallocAll, bool *aResetStats )
    {
        *aReallocAll = true;
        *aResetStats = true;

        cout << "Buffers reallocated by the pipeline" << endl;
    }

};

//
// Starts a multicast slave
//

bool StartSlave()
{
	// Let the user select the device to receive from
	PvString lDeviceIP;
	if ( !SelectDevice( lDeviceIP ) )
	{
		return false;
	}
	
	// Create the PvStream object
	PvStream lStream;

	// Create the PvPipeline object
	PvPipeline lPipeline( &lStream );

    // Create a PvPipeline event sink (used to trap buffer too small events)
    PipelineEventSink lPipelineEventSink;
    lPipeline.RegisterEventSink( &lPipelineEventSink );

	// Open stream
	cout << "Opening stream" << endl;
	lStream.Open( lDeviceIP, "239.192.1.1", 1042 );

	// IMPORTANT: the pipeline needs to be "armed", or started before 
	// we instruct the device to send us images
	cout << "Starting pipeline" << endl;
    lPipeline.SetBufferCount( 16 );
	lPipeline.Start();

	// Get stream parameters/stats
	PvGenParameterArray *lStreamParams = lStream.GetParameters();
	PvGenInteger *lCount = dynamic_cast<PvGenInteger *>( lStreamParams->Get( "ImagesCount" ) );
	PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRateAverage" ) );
	PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "BandwidthAverage" ) );
	PvGenBoolean *lIgnoreMissingPackets = dynamic_cast<PvGenBoolean *>( lStreamParams->Get( "IgnoreMissingPackets" ) );

	// Disabling resend packets
	lIgnoreMissingPackets->SetValue( true );

	char lDoodle[] = "|\\-|-/";
	int lDoodleIndex = 0;
	PvInt64 lImageCountVal = 0;
	double lFrameRateVal = 0.0;
	double lBandwidthVal = 0.0;

	// Acquire images until the user instructs us to stop
	cout << endl;
	cout << "<press a key to stop receiving>" << endl;
	while ( !PvKbHit() )
	{
		// Retrieve next buffer		
		PvBuffer *lBuffer = NULL;
        PvResult  lOperationResult;
		PvResult lResult = lPipeline.RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );
		
        if ( lResult.IsOK() )
		{
            if (lOperationResult.IsOK())
            {
                //
                // We now have a valid buffer. This is where you would typically process the buffer.
                // -----------------------------------------------------------------------------------------
                // ...

			    lCount->GetValue( lImageCountVal );
			    lFrameRate->GetValue( lFrameRateVal );
			    lBandwidth->GetValue( lBandwidthVal );
			
				// If the buffer contains an image, display width and height
				PvUInt32 lWidth = 0, lHeight = 0;
				if ( lBuffer->GetPayloadType() == PvPayloadTypeImage )
				{
					// Get image specific buffer interface
					PvImage *lImage = lBuffer->GetImage();

					// Read width, height
					lWidth = lBuffer->GetImage()->GetWidth();
					lHeight = lBuffer->GetImage()->GetHeight();
				}
			
				cout << fixed << setprecision( 1 );
				cout << lDoodle[ lDoodleIndex ];
				cout << " BlockID: " << uppercase << hex << setfill('0') << setw(16) << lBuffer->GetBlockID() << " W: " << dec << lWidth << " H: " 
				     << lHeight << " " << lFrameRateVal << " FPS " << ( lBandwidthVal / 1000000.0 ) << " Mb/s\r";
            }

            // We have an image - do some processing (...) and VERY IMPORTANT,
			// release the buffer back to the pipeline
			lPipeline.ReleaseBuffer( lBuffer );
		}
		else
		{
			// Timeout
			cout <<  lDoodle[ lDoodleIndex ] << " Timeout\r";
		}

		++lDoodleIndex %= 6;
	}

	PvGetChar(); // Flush key buffer for next stop
	cout << endl << endl;

	// We stop the pipeline - letting the object lapse out of 
	// scope would have had the destructor do the same, but we do it anyway
	cout << "Stop pipeline" << endl;
	lPipeline.Stop();

	// Now close the stream. Also optionnal but nice to have
	cout << "Closing stream" << endl;
	lStream.Close();

    // Unregister pipeline event sink. Optional but nice to have.
    lPipeline.UnregisterEventSink( &lPipelineEventSink );

	return true;
}


//
// Main function
//

int main()
{
	// Starts the multicast slave
	cout << "1. Starts the multicast slave" << endl << endl;
	StartSlave();

	cout << endl;
	cout << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

	return 0;
}

