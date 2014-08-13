// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvFilterRGB.h>
#include <PvBuffer.h>
#include <PvDevice.h>
#include <PvStream.h>
#include <PvBufferWriter.h>
#include <PvBufferConverter.h>
#ifdef PV_GUI_NOT_AVAILABLE
#include <PvSystem.h>
#else
#include <PvDeviceFinderWnd.h>
#endif // PV_GUI_NOT_AVAILABLE

PV_INIT_SIGNAL_HANDLER();

//
// Main function
//

int main()
{
	PvStream lStream;
	PvDevice lDevice;

    PvDeviceInfo *lDeviceInfo = NULL;
    PvResult lResult;

	cout << endl;

	// *********************************
	// Step 1: Select the device         
	// *********************************
    cout << "1. Select a device to connect";

#ifdef PV_GUI_NOT_AVAILABLE
	PvSystem lSystem;
    lDeviceInfo = PvSelectDevice( lSystem );
    if ( lDeviceInfo == NULL )
    {
        cout << "No device selected." << endl;
        return 0;
    }
#else
	PvDeviceFinderWnd lDeviceFinderWnd;
    lResult = lDeviceFinderWnd.ShowModal();
    if ( !lResult.IsOK() )
    {
        cout << "No device selected." << endl;
        return 0;
    }
    lDeviceInfo = lDeviceFinderWnd.GetSelected();
#endif // PV_GUI_NOT_AVAILABLE

	cout << endl;

	// ********************************
	// Step 2: Connect to the device    
	// ********************************
    cout << "2. Connecting to the selected device......";
	lResult = lDevice.Connect( lDeviceInfo, PvAccessControl );
    if ( !lResult.IsOK() )
	{ 
		cout << endl;
		cout << "  Failed to connect to device";
		return 0;
	}

    // Perform automatic packet size negociation
    lResult = lDevice.NegotiatePacketSize( );
    if ( !lResult.IsOK() )
    {
		cout << endl;
        cout << " Failed to negotiate a packet size setting GevSCPSPacketSize to original value";
        PvSleepMs( 2500 );
    }

	cout << endl;

	// *************************************************
	// Step 3: Open and set stream destination    
	// *************************************************
    cout << "3. Open stream......";
	lResult = lStream.Open( lDeviceInfo->GetIPAddress() );
	if ( !lResult.IsOK() )
	{	
		cout << endl;
		cout << "  Failed to open stream";
		return 0;
	}
	lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() );

	// Note: Set long Heartbeat Timeout for the debugging purpose
	// If the program is stopped at the breakpoint too long then the
	// hertbeat may expired

#ifdef _DEBUG
    lDevice.GetGenParameters()->SetIntegerValue( "GevHeartbeatTimeout", 60000 );
#endif

    // ******************************************************
	// Step 4: Get image size information and prepare buffer  
	// ******************************************************
	PvInt64 lPayloadSize;
	lDevice.GetGenParameters()->GetIntegerValue( "PayloadSize", lPayloadSize );

	// Create the buffer
	PvBuffer * lBuffer = new PvBuffer();

	// Alloc buffer (buffers are created as images, GetImage() is valid on a fresh buffer)
	lBuffer->Alloc( static_cast<PvUInt32>( lPayloadSize ) );

	PvBuffer *lPtr = NULL; 

	cout << endl;

	// *************************************************
	// Step 5: Grab image                               
	// *************************************************
	cout << "5. Grab one image" << endl;

	lStream.QueueBuffer( lBuffer );
	lDevice.GetGenParameters()->SetIntegerValue( "TLParamsLocked", 1 );
	lDevice.GetGenParameters()->ExecuteCommand( "AcquisitionStart" );
	PvResult lStreamResult;
	lResult = lStream.RetrieveBuffer( &lPtr, &lStreamResult, 10000 );
	lDevice.GetGenParameters()->ExecuteCommand( "AcquisitionStop" );
	lDevice.GetGenParameters()->SetIntegerValue( "TLParamsLocked", 0 );
	if ( lResult.IsOK() )
	{
		if ( lStreamResult.IsOK() )
		{
			cout << endl;
			// *************************************************
			// Step 6: Using RGB Filter                          
			// *************************************************
			cout << "6. Using RGB Filter";

			PvUInt32 lWidth = lPtr->GetImage()->GetWidth();
			PvUInt32 lHeight = lPtr->GetImage()->GetHeight();

			// Alloc buffer (buffers are created as images, GetImage() is valid on a fresh buffer)
			PvBuffer * lBufferRGB32 = new PvBuffer();
			lBufferRGB32->GetImage()->Alloc( lWidth, lHeight, PvPixelBGRa8 );

			// Alloc buffer (buffers are created as images, GetImage() is valid on a fresh buffer)
			PvBuffer * lBufferDst = new PvBuffer();
			lBufferDst->GetImage()->Alloc( lWidth, lHeight, PvPixelBGRa8 );

			PvBufferConverter lBufferConverter;
			lResult = lBufferConverter.Convert( lPtr, lBufferRGB32, true );

			// Create RGB filter, has default gains: 1 and offsets: 0
			PvFilterRGB lFilter;
			PvBufferWriter lBufferWriter;

			//Save original image
			cout << endl;
			cout << "  a. Save the original image into ImageOriginal.bmp";
			lBufferWriter.Store( lBufferRGB32, "ImageOriginal.bmp", PvBufferFormatBMP );

			// Enhance the Red
			cout << endl;
			cout << "  b. Enhance the Red color and save the image into ImageRed.bmp";
			lFilter.SetGainR( 2 );
			lFilter.SetOffsetR( 100 );
			memcpy(lBufferDst->GetDataPointer(), lBufferRGB32->GetDataPointer(), lBufferRGB32->GetSize());
			lFilter.Apply( lBufferDst );
			lBufferWriter.Store( lBufferDst, "ImageRed.bmp", PvBufferFormatBMP );
			lFilter.SetGainR(1);
			lFilter.SetOffsetR( 0 );
	
			// Enhance the Blue
			cout << endl;
			cout << "  c. Enhance the Blue color and save the image into ImageBlue.bmp";
			lFilter.SetGainB( 2 );
			lFilter.SetOffsetB( 100 );
			memcpy(lBufferDst->GetDataPointer(), lBufferRGB32->GetDataPointer(), lBufferRGB32->GetSize() );
			lFilter.Apply( lBufferDst );
			lBufferWriter.Store( lBufferDst, "ImageBlue.bmp", PvBufferFormatBMP );
			lFilter.SetGainB( 1);
			lFilter.SetOffsetB( 0 );

			// Enhance the Green
			cout << endl;
			cout << "  d. Enhance the Green color and save the imageinto ImageGreen.bmp";
			lFilter.SetGainG( 2 );
			lFilter.SetOffsetG( 100 );
			memcpy(lBufferDst->GetDataPointer(), lBufferRGB32->GetDataPointer(), lBufferRGB32->GetSize() );
			lFilter.Apply( lBufferDst );
			lBufferWriter.Store( lBufferDst, "ImageGreen.bmp", PvBufferFormatBMP );

			// Do White Balance
			cout << endl;
			cout << "  e. Do white balance and save the image into ImageBalanced.bmp" << endl;
			lFilter.WhiteBalance( lBufferRGB32 );
			lFilter.Apply( lBufferRGB32 );
			lBufferWriter.Store( lBufferRGB32, "ImageBalanced.bmp", PvBufferFormatBMP );
			lBufferRGB32->Free();
			lBufferDst->Free();
		}
	}
	
  	lBuffer->Free();
	lDevice.ResetStreamDestination();
	lStream.Close();
	lDevice.Disconnect();
		
	cout << endl;
	cout << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

	return 0;
}

