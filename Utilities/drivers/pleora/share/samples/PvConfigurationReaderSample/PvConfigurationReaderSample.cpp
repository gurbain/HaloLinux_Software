// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvConfigurationWriter.h>
#include <PvConfigurationReader.h>
#ifdef PV_GUI_NOT_AVAILABLE
#include <PvSystem.h>
#else
#include <PvDeviceFinderWnd.h>
#endif // PV_GUI_NOT_AVAILABLE

PV_INIT_SIGNAL_HANDLER();

// insert your desired file name here
#define FILE_NAME "PersistenceTest.pvxml"

//
// Choose and Connect a device
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
	cout << "Connecting to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;

	if ( !aDevice.Connect( lDeviceInfo ).IsOK() )
	{
		cout << "Unable to connect to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
		return false;
	}

	aIP = lDeviceInfo->GetIPAddress();

	cout << "Successfully connected to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
	return true;
}

//
//	Store device and stream configuration.
//	Also store a string information
//

bool StoreConfiguration()
{
	PvDevice lDevice;
	PvString lIP;
	if ( !Connect( lDevice, lIP ) )
	{
		cout << "Unable to connect the device";
		return false;
	}

	PvConfigurationWriter lWriter;

	// store with a PvDevice
	cout << "Store device configuration" << endl;
	PvString lName = "DeviceConfiguration";
	lWriter.Store( &lDevice, lName );

	// store with a PvStream
	cout << "Store stream configuration" << endl;
	PvStream lStream;
	lStream.Open( lIP );
	lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() );
	lName = "StreamConfiguration";
	lWriter.Store( &lStream, lName );
	lDevice.ResetStreamDestination();

	// Store with a sample string
	cout << "Store string information" << endl;
	PvString lString= "AnyKindOfInformation";
	lName = "StringInformation";
	lWriter.Store( lString, lName );

	// Insert the path of your file here
	cout << "Store string information" << endl;
	PvString lFilenameToSave( FILE_NAME );

	lWriter.Save( lFilenameToSave );

	lStream.Close();
	lDevice.Disconnect();

	return true;
}

//
// Restore device configuration and verify that the device is connected 
//

bool RestoreDeviceConfigurationName()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	cout << "Load information and configuration" << endl;
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the device information
	cout << "Restore configuration for a device with the configuration name" << endl;
	PvDevice lDevice;
	lReader.Restore( "DeviceConfiguration", &lDevice );

	cout << "Verify the restoration" << endl;
	if ( !lDevice.IsConnected() )
	{
		return false;
	}

	lDevice.Disconnect();

	return true;
}

//
// Restore device configuration and verify that the device is connected 
//

bool RestoreDeviceConfigurationIndex()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	cout << "Load information and configuration" << endl;
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the device information
	cout << "Restore configuration for a device with the configuration index" << endl;
	PvDevice lDevice;
	lReader.Restore( PvUInt32( 0 ), &lDevice );

	cout << "Verify the restoration" << endl;
	if ( !lDevice.IsConnected() )
	{
		return false;
	}

	lDevice.Disconnect();

	return true;
}

//
// Restore stream configuration and verify that the stream is open
//

bool RestoreStreamConfigurationName()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	cout << "Load information and configuration" << endl;
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the stream information
	cout << "Restore configuration for a stream with the configuration name" << endl;
	PvStream lStream;
	PvString lStreamConfigStr( "StreamConfiguration" );
	lReader.Restore( lStreamConfigStr, lStream );

	cout << "Verify the restoration" << endl;
	if ( !lStream.Close() )
	{
		return false;
	}

	return true;
}

//
// Restore stream configuration and verify that the stream is open
//

bool RestoreStreamConfigurationIndex()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	cout << "Load information and configuration" << endl;
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the stream information
	cout << "Restore configuration for a stream with the configuration index" << endl;
	PvStream lStream;
	lReader.Restore( PvUInt32( 0 ), lStream );

	cout << "Verify the restoration" << endl;
	if ( !lStream.Close() )
	{
		return false;
	}

	return true;
}

//
// Restore string information and verify that the string is the same
//

bool RestoreStringInformationName()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	cout << "Load information and configuration" << endl;
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the stream information
	cout << "Restore information for a string with the information name" << endl;
	PvString lCheck;
	PvString lStringInfoStr( "StringInformation" );
	lReader.Restore( lStringInfoStr, lCheck );

	cout << "Verify the restoration" << endl;
	if ( lCheck != "AnyKindOfInformation" )
	{
		return false;
	}

	return true;
}

//
// Restore string information and verify that the string is the same
//

bool RestoreStringInformationIndex()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	cout << "Load information and configuration" << endl;
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the stream information
	cout << "Restore information for a string with the information index" << endl;
	PvString lCheck;
	lReader.Restore( PvUInt32( 0 ), lCheck );

	cout << "Verify the restoration" << endl;
	if ( lCheck != "AnyKindOfInformation" )
	{
		return false;
	}

	return true;
}

//
// Main function
//

int main()
{
	// Create the Buffer and fill it
	cout << endl;
	cout << "1. Store the configuration" << endl << endl;
	if ( !StoreConfiguration() )
	{
		cout << "Cannot store the configuration correctly";
		return 0;
	}

	PvSleepMs( 5000 );

	cout << endl;
	cout << "2. Restore the device configuration with a name" << endl << endl;
	if ( !RestoreDeviceConfigurationName() )
	{
		cout << "Cannot restore the configuration correctly";
		return 0;
	}

	PvSleepMs( 5000 );

	cout << endl;
	cout << "3. Restore the device configuration with an index" << endl << endl;
	if ( !RestoreDeviceConfigurationIndex() )
	{
		cout << "Cannot restore the configuration correctly";
		return 0;
	}

	PvSleepMs( 5000 );

	cout << endl;
	cout << "4. Restore the stream configuration with a name" << endl << endl;
	if ( !RestoreStreamConfigurationName() )
	{
		cout << "Cannot restore the configuration correctly";
		return 0;
	}

	PvSleepMs( 5000 );

	cout << endl;
	cout << "5. Restore the stream configuration with an index" << endl << endl;
	if ( !RestoreStreamConfigurationIndex() )
	{
		cout << "Cannot restore the configuration correctly";
		return 0;
	}

	cout << endl;
	cout << "6. Restore the string information with a name" << endl << endl;
	if ( !RestoreStringInformationName() )
	{
		cout << "Cannot restore the configuration correctly";
		return 0;
	}

	cout << endl;
	cout << "7. Restore the string information with an index" << endl << endl;
	if ( !RestoreStringInformationIndex() )
	{
		cout << "Cannot restore the configuration correctly";
		return 0;
	}

	cout << endl;
	cout << "<press a key to exit>" << endl;
	PvWaitForKeyPress();

	return 0;
}

