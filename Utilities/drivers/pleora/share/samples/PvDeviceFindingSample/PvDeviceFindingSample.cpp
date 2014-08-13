// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

// 
// This sample shows how to use PvDeviceFinder to find GEV Devices on a network 
// using two different methods.
// 1. Finding Pleora GigE Vision devices programatically 
// 2. Finding Pleora GigE Vision devices using a GUI
// 

#include <PvSampleUtils.h>
#include <PvSystem.h>
#include <PvInterface.h>
#include <PvDevice.h>
#ifndef PV_GUI_NOT_AVAILABLE
#include <PvDeviceFinderWnd.h>
#endif // PV_GUI_NOT_AVAILABLE

PV_INIT_SIGNAL_HANDLER();

//
// To find GEV Devices on a network
//

int GEVDeviceFinding()
{
	PvResult lResult;	
	PvDeviceInfo *lDeviceInfo = 0;

	// Create an GEV system and an interface.
	PvSystem lSystem;

	// Find all GEV Devices on the network.
	lSystem.SetDetectionTimeout( 2000 );
	lResult = lSystem.Find();
	if( !lResult.IsOK() )
	{
		cout << "PvSystem::Find Error: " << lResult.GetCodeString().GetAscii();
		return -1;
	}

	// Get the number of GEV Interfaces that were found using GetInterfaceCount.
	PvUInt32 lInterfaceCount = lSystem.GetInterfaceCount();

	// Display information about all found interface
	// For each interface, display information about all devices.
	for( PvUInt32 x = 0; x < lInterfaceCount; x++ )
	{
		// get pointer to each of interface
		PvInterface * lInterface = lSystem.GetInterface( x );

		cout << "Interface " << x << endl;
		cout << "MAC Address: " << lInterface->GetMACAddress().GetAscii() << endl;
		cout << "IP Address: " << lInterface->GetIPAddress().GetAscii() << endl;
		cout << "Subnet Mask: " << lInterface->GetSubnetMask().GetAscii() << endl << endl;

		// Get the number of GEV devices that were found using GetDeviceCount.
		PvUInt32 lDeviceCount = lInterface->GetDeviceCount();

		for( PvUInt32 y = 0; y < lDeviceCount ; y++ )
		{
			lDeviceInfo = lInterface->GetDeviceInfo( y );

			cout << "Device " << y << endl;
			cout << "MAC Address: " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
			cout << "IP Address: " << lDeviceInfo->GetIPAddress().GetAscii() << endl;
			cout << "Serial number: " << lDeviceInfo->GetSerialNumber().GetAscii() << endl << endl;
		}
	}

	// Connect to the last GEV Device found.
	if( lDeviceInfo != NULL )
	{
		cout << "Connecting to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;

		PvDevice lDevice;
		lResult = lDevice.Connect( lDeviceInfo );
		if ( !lResult.IsOK() )
		{
			cout << "Unable to connect to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
		}
		else
		{
			cout << "Successfully connected to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
		}
	}
	else
	{
		cout << "No device found" << endl;
	}

	return 0;
}

#ifndef PV_GUI_NOT_AVAILABLE
//
// To find GEV Devices on a network (using a GUI)
//

int GUIGEVDeviceFinding()
{
	// Create a GUI GEV Device finder. Use PvDeviceFinderWnd::PvDeviceFinderWnd.
	PvDeviceFinderWnd lDeviceFinderWnd;

	// Prompt the user to select a GEV Device. Use PvDeviceFinderWnd::ShowModal.
	if ( !lDeviceFinderWnd.ShowModal().IsOK() )
	{
		// Error can include dialog cancel as well
		return -1;
	}

	// When dismissed with OK, the device finder dialog keeps a reference
	// on the device that was selected by the user. Retrieve this reference.
	PvDeviceInfo* lDeviceInfo = lDeviceFinderWnd.GetSelected();

	// Connect to the selected GEV Device.
	if( lDeviceInfo != NULL )
	{
		cout << "Connecting to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;

		PvDevice lDevice;
		PvResult lResult = lDevice.Connect( lDeviceInfo );
		if ( !lResult.IsOK() )
		{
			cout << "Unable to connect to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
		}
		else
		{
			cout << "Successfully connected to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
		}
	}
	else
	{
		cout << "No device selected" << endl;
	}

	return 0;
}
#endif // PV_GUI_NOT_AVAILABLE

//
// Main function
//

int main()
{
	// Find devices programmatically
	cout << "1. Find devices programmatically" << endl << endl;
	GEVDeviceFinding();

#ifndef PV_GUI_NOT_AVAILABLE
	cout << endl;
	// Prompt the user to select a device using the GUI
	cout << "2. Prompt the user to select a device using the GUI" << endl << endl;
	GUIGEVDeviceFinding();
#endif // PV_GUI_NOT_AVAILABLE

	cout << endl;
	cout << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

	return 0;
}

