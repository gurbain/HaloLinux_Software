// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// Multicasting master
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
// Have the user select a device and connect the PvDevice object
// to the user's selection.
//

bool Connect( PvDevice &aDevice, PvString &aIP )
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
	cout << "Connecting to to " <<  lDeviceInfo->GetMACAddress().GetAscii() << endl;

	if ( !aDevice.Connect( lDeviceInfo ).IsOK() )
	{
		cout << "Unable to connect to " <<  
			lDeviceInfo->GetMACAddress().GetAscii() << endl;
		return false;
	}
	cout << "Successfully connected to " <<  
			lDeviceInfo->GetMACAddress().GetAscii() << endl;

	aIP = lDeviceInfo->GetIPAddress();

	return true;
}


//
// Shows how to control a multicast master without receiving data
//

bool StartMaster()
{
	// Connect to the GEV Device.
	PvString lDeviceIP;
	PvDevice lDevice;
	if ( !Connect( lDevice, lDeviceIP ) )
	{
		return false;
	}

	cout << endl;

	bool lCheck = lDevice.IsConnected();

	PvResult lResult = lDevice.SetStreamDestination( "239.192.1.1", 1042 );

	// Get device parameters need to control streaming
	PvGenParameterArray *lDeviceParams = lDevice.GetGenParameters();
	PvGenInteger *lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
	PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
	PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );
	PvGenInteger *lGevSCPSPacketSize = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "GevSCPSPacketSize" ) );

	// Auto packet size negotiation does not happen with multicasting, here
	// we set it to small packets in order to be on the safe side
	lGevSCPSPacketSize->SetValue( 1440 );

	// TLParamsLocked is optional but when present, it MUST be set to 1
	// before sending the AcquisitionStart command
	if ( lTLLocked != NULL )
	{
		cout << "Setting TLParamsLocked to 1" << endl;
		lTLLocked->SetValue( 1 );
	}

	// Tell the device to start sending images to the multicast group
	cout << "Sending StartAcquisition command to device" << endl;
	lStart->Execute();

	// Acquire images until the user instructs us to stop
	cout << endl;
	cout << "<press a key to stop streaming>" << endl;
    PvWaitForKeyPress();

	PvGetChar(); // Flush key buffer for next stop
	cout << endl;

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
	// Connect to device and start streaming as a multicast master
	cout << "1. Connect to device and start streaming as a multicast master" << endl << endl;
	StartMaster();

	cout << endl;
	cout << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

	return 0;
}

