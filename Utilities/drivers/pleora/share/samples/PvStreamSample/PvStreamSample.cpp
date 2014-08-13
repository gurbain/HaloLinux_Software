// *****************************************************************************
//
//     Copyright (c) 2010, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// To receive images using PvStream directly
//

#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvBuffer.h>
#include <PvStream.h>
#ifdef PV_GUI_NOT_AVAILABLE
#include <PvSystem.h>
#else
#include <PvDeviceFinderWnd.h>
#endif // PV_GUI_NOT_AVAILABLE

PV_INIT_SIGNAL_HANDLER();

#define BUFFER_COUNT ( 16 )

//
// Shows how to use a PvPipeline object to acquire images from a device
//

bool AcquireImages()
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

    // Connect to the GEV Device
    PvDevice lDevice;
    cout << "Connecting to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
    if ( !lDevice.Connect( lDeviceInfo ).IsOK() )
    {
        cout << "Unable to connect to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
        return false;
    }
    cout << "Successfully connected to " << lDeviceInfo->GetMACAddress().GetAscii() << endl << endl;

    // Get device parameters need to control streaming
    PvGenParameterArray *lDeviceParams = lDevice.GetGenParameters();
    PvGenInteger *lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
    PvGenInteger *lPayloadSize = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "PayloadSize" ) );
    PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
    PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

    // Negotiate streaming packet size
    lDevice.NegotiatePacketSize();

    // Create the PvStream object
    PvStream lStream;

    cout << "Gabriel says: " << lDeviceInfo->GetIPAddress().GetAscii() << endl;

    // Open stream - have the PvDevice do it for us
    cout << "Opening stream to device" << endl;
    lStream.Open( lDeviceInfo->GetIPAddress() );

    // Reading payload size from device
    PvInt64 lSize = 0;
    lPayloadSize->GetValue( lSize );

    // Use min of BUFFER_COUNT and how many buffers can be queued in PvStream
    PvUInt32 lBufferCount = ( lStream.GetQueuedBufferMaximum() < BUFFER_COUNT ) ? 
        lStream.GetQueuedBufferMaximum() : 
        BUFFER_COUNT;

    // Create, alloc buffers
    PvBuffer *lBuffers = new PvBuffer[ lBufferCount ];
    for ( PvUInt32 i = 0; i < lBufferCount; i++ )
    {
        lBuffers[ i ].Alloc( static_cast<PvUInt32>( lSize ) );
    }

    // Have to set the Device IP destination to the Stream
    lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() );

    // Get stream parameters/stats
    PvGenParameterArray *lStreamParams = lStream.GetParameters();
    PvGenInteger *lCount = dynamic_cast<PvGenInteger *>( lStreamParams->Get( "ImagesCount" ) );
    PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRateAverage" ) );
    PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "BandwidthAverage" ) );

    // Queue all buffers in the stream
    for ( PvUInt32 i = 0; i < lBufferCount; i++ )
    {
        lStream.QueueBuffer( lBuffers + i );
    }

    // TLParamsLocked is optional but when present, it MUST be set to 1
    // before sending the AcquisitionStart command
    if ( lTLLocked != NULL )
    {
        cout << "Setting TLParamsLocked to 1" << endl;
        lTLLocked->SetValue( 1 );
    }

    cout << "Resetting timestamp counter..." << endl;
    PvGenCommand *lResetTimestamp = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "GevTimestampControlReset" ) );
    lResetTimestamp->Execute();

    // The buffers are queued in the stream, we just have to tell the device
    // to start sending us images
    cout << "Sending StartAcquisition command to device" << endl;
    lResult = lStart->Execute();

    char lDoodle[] = "|\\-|-/";
    int lDoodleIndex = 0;
    PvInt64 lImageCountVal = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;

	cout << endl;
    // Acquire images until the user instructs us to stop
    cout << "<press a key to stop streaming>" << endl;
    while ( !PvKbHit() )
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;

        // Retrieve next buffer		
        PvResult lResult = lStream.RetrieveBuffer( &lBuffer, &lOperationResult, 1000 );
        if ( lResult.IsOK() )
        {
        	if(lOperationResult.IsOK())
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
					 << lHeight << " " << lFrameRateVal << " FPS " << ( lBandwidthVal / 1000000.0 ) << " Mb/s  \r";
        	}
			// We have an image - do some processing (...) and VERY IMPORTANT,
			// re-queue the buffer in the stream object
			lStream.QueueBuffer( lBuffer );

        }
        else
        {
            // Timeout
            cout << lDoodle[ lDoodleIndex ] << " Timeout\r";
        }

        ++lDoodleIndex %= 6;
    }

    PvGetChar(); // Flush key buffer for next stop
    cout << endl << endl;

    // Tell the device to stop sending images
    cout << "Sending AcquisitionStop command to the device" << endl;
    lStop->Execute();

    // If present reset TLParamsLocked to 0. Must be done AFTER the 
    // streaming has been stopped
    if ( lTLLocked != NULL )
    {
        cout << "Resetting TLParamsLocked to 0" << endl;
        lTLLocked->SetValue( 0 );
    }

    // Abort all buffers from the stream, unqueue
    cout << "Aborting buffers still in stream" << endl;
    lStream.AbortQueuedBuffers();
    while ( lStream.GetQueuedBufferCount() > 0 )
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;

        lStream.RetrieveBuffer( &lBuffer, &lOperationResult );

        cout << "  Post-abort retrieved buffer: " << lOperationResult.GetCodeString().GetAscii() << endl;
    }

    // Release buffers
    cout << "Releasing buffers" << endl;
    delete []lBuffers;

    // Now close the stream. Also optionnal but nice to have
    cout << "Closing stream" << endl;
    lStream.Close();

    // Finally disconnect the device. Optional, still nice to have
    cout << "Disconnecting device" << endl;
    lDevice.Disconnect();

    return true;
}


//
// Main function
//

int main()
{
    // PvPipeline used to acquire images from a device
    cout << "1. PvStream sample - image acquisition from a device" << endl << endl;
    AcquireImages();

	cout << endl;
    cout << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

    return 0;
}

