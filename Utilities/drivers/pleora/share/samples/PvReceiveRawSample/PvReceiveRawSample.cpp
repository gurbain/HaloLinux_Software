// *****************************************************************************
//
// Copyright (c) 2011, Pleora Technologies Inc., All rights reserved.
//
// -----------------------------------------------------------------------------
// This sample code illustrates how to use the PvStream and PvPipeline classes 
// to receive raw data using the eBUS SDK.
//
// Using the PvDeviceFinderWnd class, this sample enumerates available GEV 
// devices to the user and allows the user to select a GEV device. Note that the 
// selected device must transmit raw data on channel 0.
// Using the PvStream and PvPipeline classes, this sample illustrates how to 
// receive raw data from a eBUS Transmitter.
// A PvBuffer is used to hold the raw data once it is received.
//
// By default, this sample receives raw buffers passively from multicast address 
// 239.192.1.1:1042. However, it can be used in many different ways by providing 
// it with optional command line arguments. For further details on the command 
// line options that are available, run the sample with the argument --help.
//
// Also note that by default, this sample receives data passively (it connects 
// as a pure data receiver). However, if the --connectdevice command line argument 
// is supplied, a PvDevice is instantiated to control the transmitting entity and 
// start streaming. This mode of operation is not supported by all eBUS Transmitters.
//
// Refer to the readme in PvTransmitRawSample for how to test transmission and 
// reception of raw buffers by having PvReceiveRawSample receive data from 
// PvTransmitRawSample.
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvSystem.h>
#include <PvInterface.h>
#include <PvStream.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#ifdef PV_GUI_NOT_AVAILABLE
#include <PvSystem.h>
#else
#include <PvDeviceFinderWnd.h>
#endif // PV_GUI_NOT_AVAILABLE

#include "Configuration.h"

PvUInt64 GetTickCountMs();
void SleepMs( PvUInt32 aSleepTime );
void PrintHelp();

PV_INIT_SIGNAL_HANDLER();

int main( int aCount, const char ** aArgs )
{
    Config lConfig;
    lConfig.ParseCommandLine( aCount, aArgs );

    if ( lConfig.GetDeviceAddress().length() <= 0 )
    {
        PvDeviceInfo *lDI = NULL;
        PvResult lResult;

    #ifdef PV_GUI_NOT_AVAILABLE
	    PvSystem lSystem;
        lDI = PvSelectDevice( lSystem );
        if( lDI == NULL )
        {
            cout << "No device selected." << endl;
            return 1;
        }
    #else
	    PvDeviceFinderWnd lDeviceFinderWnd;
        lResult = lDeviceFinderWnd.ShowModal();

        if( !lResult.IsOK() )
        {
            cout << "No device selected." << endl;
            return 1;

        }
        lDI = lDeviceFinderWnd.GetSelected();
    #endif // PV_GUI_NOT_AVAILABLE

        lConfig.SetDeviceAddress( lDI->GetIPAddress().GetAscii() );
    }

    PvStream lStream;

    PvResult lResult;
    if ( lConfig.GetUnicast() )
    {
        lResult = lStream.Open( 
            lConfig.GetDeviceAddress().c_str(), lConfig.GetHostPort(), 
            lConfig.GetChannel(), lConfig.GetLocalAddress().c_str() );
        cout << "Receiving from device " << lConfig.GetDeviceAddress() << " ";
        cout << "on interface " << lStream.GetLocalIPAddress().GetAscii() << ":" << lStream.GetLocalPort() << endl;
    }
    else
    {
        lResult = lStream.Open( lConfig.GetDeviceAddress().c_str(), lConfig.GetMulticastAddress().c_str(), 
            lConfig.GetHostPort(), lConfig.GetChannel(), lConfig.GetLocalAddress().c_str() );
        cout << "Receiving from multicast address " << lConfig.GetMulticastAddress().c_str() << ":" << lConfig.GetHostPort() << " ";
        cout << "(device " << lConfig.GetDeviceAddress().c_str() << ") ";
        cout << "on interface " << lStream.GetLocalIPAddress().GetAscii() << ":" << lStream.GetLocalPort() << endl;
    }

    if ( !lResult.IsOK() )
    {
        cout << "Failed opening the incoming stream: " << lResult.GetDescription().GetAscii() << endl;
        return 1;
    }

    PvPipeline lPipeline( &lStream );

    PvDevice lDevice;
    PvGenParameterArray *lDeviceParams = NULL;
    if ( lConfig.GetConnectDevice() )
    {
        lResult = lDevice.Connect( lConfig.GetDeviceAddress().c_str() );
        if ( !lResult.IsOK() )
        {
            cout << "Failed connecting to the device to set its destination and initiate an AcquisitionStart: ";
            cout << lResult.GetDescription().GetAscii() << endl;
            return 1;
        }
        lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort(), lConfig.GetChannel() );
            
        // Get device parameters need to control streaming
        lDeviceParams = lDevice.GetGenParameters();

        // Reading payload size from device. Otherwise, the pipeline may miss the first several images.
        PvInt64 lReceivePayloadSize = 0;
	    lDeviceParams->GetIntegerValue( "PayloadSize", lReceivePayloadSize );

        // Set the buffer size
        lPipeline.SetBufferSize( static_cast<PvUInt32>( lReceivePayloadSize ) );
    }

    // Set buffer count, start the pipeline
    lPipeline.SetBufferCount( 16 );
    lPipeline.Start();

    if ( lConfig.GetConnectDevice() )
    {
        // TLParamsLocked is optional but when present, it MUST be set to 1
        // before sending the AcquisitionStart command
	    lDeviceParams->SetIntegerValue( "TLParamsLocked", 1 );

	    lDeviceParams->ExecuteCommand( "GevTimestampControlReset" );

        // The pipeline is already "armed", we just have to tell the device
        // to start sending us images
	    lDeviceParams->ExecuteCommand( "AcquisitionStart" );
    }
    
    // Get stream parameters/stats
    PvGenParameterArray *lStreamParams = lStream.GetParameters();

    cout << "Press any key to stop receiving." << endl;

    char lDoodle[] = "|\\-|-/";
	int lDoodleIndex = 0;

	PvInt64 lBlockCountVal = 0;

	double lAcqRateVal = 0.0;
    double lAcqRateAvgVal = 0.0;
    double lBandwidthVal = 0.0;
    double lBandwidthAvgVal = 0.0;

    while ( !PvKbHit() )
    {
        PvBuffer *lBuffer = NULL;
        PvResult  lOperationResult;
        PvResult lResult = lPipeline.RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );
        
        if ( lResult.IsOK() )
        {
            if ( lOperationResult.IsOK() )
            {
                //
                // We now have a valid buffer. This is where you would typically process the buffer.
                // -----------------------------------------------------------------------------------------
                // ...

                lStreamParams->GetIntegerValue( "BlockCount", lBlockCountVal );
                lStreamParams->GetFloatValue( "AcquisitionRate", lAcqRateVal );
                lStreamParams->GetFloatValue( "AcquisitionRateAverage", lAcqRateAvgVal );
                lStreamParams->GetFloatValue( "Bandwidth", lBandwidthVal );
                lStreamParams->GetFloatValue( "BandwidthAverage", lBandwidthAvgVal );

				cout << fixed << setprecision( 1 );
				cout << lDoodle[ lDoodleIndex ] << " ";
				cout << "Received " << lBlockCountVal << " blocks ";
				cout << "at " << lAcqRateAvgVal << " ";
				cout << "(" << lAcqRateVal << ") FPS ";
				cout << lBandwidthAvgVal / 1000000.0f << " ";
				cout << "(" << lBandwidthVal / 1000000.0f << ") Mb/s  \r";
		        ++lDoodleIndex %= 6;
            }

            // We have an image - do some processing (...) and VERY IMPORTANT,
            // release the buffer back to the pipeline
            lPipeline.ReleaseBuffer( lBuffer );
        }
        else
        {
			cout << lDoodle[ lDoodleIndex ] << " Timeout\r";
        }
    }

    if ( lConfig.GetConnectDevice() )
    {
	    lDeviceParams->ExecuteCommand( "AcquisitionStop" );
	    lDeviceParams->SetIntegerValue( "TLParamsLocked", 0 );
    }

    lPipeline.Stop();
    lStream.Close();

    if ( lConfig.GetConnectDevice() )
    {
        lDevice.Disconnect();
    }
}

