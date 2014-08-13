// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvPixelType.h>
#include <PvBufferWriter.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvStream.h>
#ifdef PV_GUI_NOT_AVAILABLE
#include <PvSystem.h>
#else
#include <PvDeviceFinderWnd.h>
#endif // PV_GUI_NOT_AVAILABLE

PV_INIT_SIGNAL_HANDLER();

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

    // Negotiate streaming packet size
    lDevice.NegotiatePacketSize();

    // Create the PvStream object
    PvStream lStream;

    // Open stream - have the PvDevice do it for us
    cout << "Opening stream to device" << endl;
    lStream.Open( lDeviceInfo->GetIPAddress() );

    // Create the PvPipeline object
    PvPipeline lPipeline( &lStream );
    
    // Reading payload size from device
    PvInt64 lSize = 0;
	lDeviceParams->GetIntegerValue( "PayloadSize", lSize );

    // Set the Buffer size and the Buffer count
    lPipeline.SetBufferSize( static_cast<PvUInt32>( lSize ) );
    lPipeline.SetBufferCount( 16 ); // Increase for high frame rate without missing block IDs

    // Have to set the Device IP destination to the Stream
    lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() );

    // IMPORTANT: the pipeline needs to be "armed", or started before 
    // we instruct the device to send us images
    cout << "Starting pipeline" << endl;
    lPipeline.Start();

    // Get stream parameters/stats
    PvGenParameterArray *lStreamParams = lStream.GetParameters();

    // TLParamsLocked is optional but when present, it MUST be set to 1
    // before sending the AcquisitionStart command
	lDeviceParams->SetIntegerValue( "TLParamsLocked", 1 );

    cout << "Resetting timestamp counter..." << endl;
	lDeviceParams->ExecuteCommand( "GevTimestampControlReset" );

    // The pipeline is already "armed", we just have to tell the device
    // to start sending us images
    cout << "Sending StartAcquisition command to device" << endl;
	lDeviceParams->ExecuteCommand( "AcquisitionStart" );

    char lDoodle[] = "|\\-|-/";
    int lDoodleIndex = 0;
    PvInt64 lImageCountVal = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;

    // Acquire images until the user instructs us to stop
	cout << endl;
    cout << "<press a key to stop streaming>" << endl;
    while ( !PvKbHit() )
    {
        // Retrieve next buffer		
        PvBuffer *lBuffer = NULL;
        PvResult  lOperationResult;
        lResult = lPipeline.RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );
        
        if ( lResult.IsOK() )
        {
            if ( lOperationResult.IsOK() )
            {
                //
                // We now have a valid buffer. This is where you would typically process the buffer.
                // -----------------------------------------------------------------------------------------
                // ...

				lStreamParams->GetIntegerValue( "ImagesCount", lImageCountVal );
				lStreamParams->GetFloatValue( "AcquisitionRateAverage", lFrameRateVal );
				lStreamParams->GetFloatValue( "BandwidthAverage", lBandwidthVal );
            
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
		// Construct the file name
		//Save jpg images
	stringstream ss;
	string name = "Image";
	string type = ".bmp";
	ss<<name<<lImageCountVal<<type;
	string filename = ss.str();
	ss.str("");		
	PvString lFilename( filename.c_str() );
			PvString lCompleteFileName( lFilename );
			string lCheck = lCompleteFileName.GetAscii();

			// Create the PvBufferWriter
			PvBufferWriter lBufferWriter;
			PvResult lResult = lBufferWriter.Store( lBuffer, lCompleteFileName, PvBufferFormatBMP );
            // We have an image - do some processing (...) and VERY IMPORTANT,
            // release the buffer back to the pipeline
            lPipeline.ReleaseBuffer( lBuffer );
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
	lDeviceParams->ExecuteCommand( "AcquisitionStop" );

    // If present reset TLParamsLocked to 0. Must be done AFTER the 
    // streaming has been stopped
	lDeviceParams->SetIntegerValue( "TLParamsLocked", 0 );

    // We stop the pipeline - letting the object lapse out of 
    // scope would have had the destructor do the same, but we do it anyway
    cout << "Stop pipeline" << endl;
    lPipeline.Stop();

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
    cout << "1. PvPipeline sample - image acquisition from a device" << endl << endl;
    AcquireImages();

	cout << endl;
    cout << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

    return 0;
}

