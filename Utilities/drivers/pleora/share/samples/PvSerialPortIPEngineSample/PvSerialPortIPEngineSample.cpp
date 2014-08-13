// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// To receive images using PvPipeline
//

#include <PvSampleUtils.h>
#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>
#include <PvSerialPortIPEngine.h>

PV_INIT_SIGNAL_HANDLER();

// #define UART0
#define BULK0

#define TEST_COUNT ( 16 )

//
// Shows how to use a PvSerialPortIPEngine to write data to an IP engine and continuously read it back
// (with the IP engine serial port set in loopback mode).
//

bool TestSerialCommunications()
{
    PvResult lResult;
    PvDeviceInfo *lDeviceInfo = NULL;

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

    // If no device is selected, abort
    if( lDeviceInfo == NULL )
    {
        cout << "No device selected." << endl;
        return false;
    }

    // Connect to the GEV Device
    PvDevice lDevice;
    cout << "Connecting to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
    if ( !lDevice.Connect( lDeviceInfo ).IsOK() )
    {
        cout << "Unable to connect to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
        return false;
    }
    cout << "Successfully connected to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;

    // Get device parameters need to control streaming
    PvGenParameterArray *lParams = lDevice.GetGenParameters();

	// Configure serial port - this is done directly on the device GenICam interface, not 
	// on the serial port object! 
#ifdef UART0
	lParams->SetEnumValue( "Uart0BaudRate", "Baud9600" );
	lParams->SetEnumValue( "Uart0NumOfStopBits", "One" );
	lParams->SetEnumValue( "Uart0Parity", "None" );

	// For this test to work without attached serial hardware we enable the port loopback
	lParams->SetBooleanValue( "Uart0Loopback", true );
#endif
#ifdef BULK0
	lParams->SetEnumValue( "BulkSelector", "Bulk0" );
	lParams->SetEnumValue( "BulkMode", "UART" );
	lParams->SetEnumValue( "BulkBaudRate", "Baud9600" );
	lParams->SetEnumValue( "BulkNumOfStopBits", "One" );
	lParams->SetEnumValue( "BulkParity", "None" );

	// For this test to work without attached serial hardware we enable the port loopback
	lParams->SetBooleanValue( "BulkLoopback", true );
#endif // BULK0

	// Open serial port
	PvSerialPortIPEngine lPort;
#ifdef UART0
	lResult = lPort.Open( &lDevice, PvIPEngineSerial0 );
#endif // UART0
#ifdef BULK0
	lResult = lPort.Open( &lDevice, PvIPEngineSerialBulk0 );
#endif // BULK0
	if ( !lResult.IsOK() )
	{
		cout << "Unable to open serial port on device: " << lResult.GetCodeString().GetAscii() << " " <<  lResult.GetDescription().GetAscii() << endl;
		return false;
	}
	cout << "Serial port opened" << endl;

	// Make sure the PvSerialPortIPEngine receive queue is big enough
	lPort.SetRxBufferSize( 2 << TEST_COUNT );

	PvUInt32 lSize = 1;
	for ( int lCount = 0; lCount < TEST_COUNT; lCount++ )
	{
		// Allocate test buffers
		unsigned char *lInBuffer = new unsigned char[ lSize ];
		unsigned char *lOutBuffer = new unsigned char[ lSize ];

		// Fill input buffer with random data
		for ( PvUInt32 i = 0; i < lSize; i++ )
		{
			lInBuffer[ i ] = ::rand() % 256;		
		}

		// Send the buffer content on the serial port
		PvUInt32 lBytesWritten = 0;
		lResult = lPort.Write( lInBuffer, lSize, lBytesWritten );
		if ( !lResult.IsOK() )
		{
			// Unable to send data over serial port!
			cout << "Error sending data over the serial port: " << lResult.GetCodeString().GetAscii() << " " <<  lResult.GetDescription().GetAscii() << endl;
			return false;
		}

		cout << "Sent " << lBytesWritten << " bytes through the serial port" << endl;

		//
		// Wait until we have all the bytes or we timeout. The Read method only times out
		// if not data is available when the function call occurs, otherwise it returns
		// all the currently available data. It is possible we have to call Read multiple
		// times to retrieve all the data if all the expected data hasn't been received yet.
		//
		// Your own code driving a serial protocol should check for a message being complete,
		// whether it is on some sort of EOF or length. You should pump out data until you
		// have what you are waiting for or reach some timeout.
		//
		PvUInt32 lTotalBytesRead = 0;
		while ( lTotalBytesRead < lSize )
		{
			PvUInt32 lBytesRead = 0;
			lResult = lPort.Read( lOutBuffer + lTotalBytesRead, lSize - lTotalBytesRead, lBytesRead, 100 );
			if ( lResult.GetCode() == PvResult::Code::TIMEOUT )
			{
				cout << "Timeout" << endl;
				break;
			}

			// Increement read head
			lTotalBytesRead += lBytesRead;
		}

		// Validate answer
		if ( lTotalBytesRead != lBytesWritten )
		{
			// Did not receive all expected bytes
			cout << "Only received " << lTotalBytesRead << " out of " << lBytesWritten << " bytes" << endl;
		}
		else
		{
			// Compare input and output buffers
			PvUInt32 lErrorCount = 0;
			for ( PvUInt32 i = 0; i < lBytesWritten; i++ )
			{
				if ( lInBuffer[ i ] != lOutBuffer[ i ] )
				{
					lErrorCount++;
				}
			}

			// Display error count
			cout << "Error count: " << lErrorCount << endl;
		}

		// Free test buffers
		delete []lInBuffer;
		delete []lOutBuffer;

		// Grow test case
		lSize *= 2;
	}

	// Close serial port
	lPort.Close();
	cout << "Serial port closed" << endl;

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
    // PvSerialIPEngine used to perform serial comm through a GigE VIsion IP Engine
    cout << "1. PvSerialIPEngine sample - serial communication through a GigE Vision IP Engine" << endl << endl;
    TestSerialCommunications();

    cout << endl << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

    return 0;
}
