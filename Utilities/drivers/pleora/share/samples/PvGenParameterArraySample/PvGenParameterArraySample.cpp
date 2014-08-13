// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// This sample shows you how to control features programatically.
//

#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvGenParameterArray.h>
#include <PvGenParameter.h>
#include <PvStream.h>
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
	cout << "Connecting to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
	if ( !aDevice.Connect( lDeviceInfo, PvAccessControl ).IsOK() )
	{
		cout << "Unable to connect to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;
		return false;
	}
	cout << "Successfully connected to " << lDeviceInfo->GetMACAddress().GetAscii() << endl;

	aIP = lDeviceInfo->GetIPAddress();

	return true;
}

//
// Dumps the full content of a PvGenParameterArray
//

bool DumpGenParameterArray( PvGenParameterArray *aArray )
{
	// Getting array size
	PvUInt32 lParameterArrayCount = aArray->GetCount();
	cout << endl;
	cout << "Array has " << lParameterArrayCount << " parameters" << endl;

	// Traverse through Array and print out parameters available
	for( PvUInt32 x = 0; x < lParameterArrayCount; x++ )
	{
		// Get a parameter
		PvGenParameter *lGenParameter = aArray->Get( x );

        // Don't show invisible parameters - display everything up to Guru
        if ( !lGenParameter->IsVisible( PvGenVisibilityGuru ) )
        {
            continue;
        }

		// Get and print parameter's name
		PvString lGenParameterName, lCategory;
		lGenParameter->GetCategory( lCategory );
		lGenParameter->GetName( lGenParameterName );
		cout << lCategory.GetAscii() << ":" << lGenParameterName.GetAscii() << ", ";

        // Parameter available?
        if ( !lGenParameter->IsAvailable() )
        {
            cout << "{Not Available}" << endl;
			continue;
        }

		// Parameter readable?
		if ( !lGenParameter->IsReadable() )
		{
            cout << "{Not readable}" << endl;
			continue;
		}
		
		// Get the parameter type
		PvGenType lType;
		lGenParameter->GetType( lType );
		switch ( lType )
		{
			// If the parameter is of type PvGenTypeInteger
			case PvGenTypeInteger:
				{
					PvInt64 lValue;				
					static_cast<PvGenInteger *>( lGenParameter )->GetValue( lValue );
					cout << "Integer: " << lValue;
				}
				break;
			// If the parameter is of type PvGenTypeEnum
			case PvGenTypeEnum:
				{
					PvString lValue;				
					static_cast<PvGenEnum *>( lGenParameter )->GetValue( lValue );
					cout << "Enum: " << lValue.GetAscii();
				}
				break;
			// If the parameter is of type PvGenTypeBoolean
			case PvGenTypeBoolean:
				{
					bool lValue;				
					static_cast<PvGenBoolean *>( lGenParameter )->GetValue( lValue );
					if( lValue ) 
					{
						cout << "Boolean: TRUE";
					}
					else 
					{
						cout << "Boolean: FALSE";
					}
				}
				break;
			// If the parameter is of type PvGenTypeString
			case PvGenTypeString:
				{
					PvString lValue;
					static_cast<PvGenString *>( lGenParameter )->GetValue( lValue );
					cout << "String: " << lValue.GetAscii();
				}
				break;
			// If the parameter is of type PvGenTypeCommand
			case PvGenTypeCommand:
				cout << "Command";
				break;
			// If the parameter is of type PvGenTypeFloat
			case PvGenTypeFloat:
				{
					double lValue;				
					static_cast<PvGenFloat *>( lGenParameter )->GetValue( lValue );
					cout << "Float: " << lValue;
				}
				break;
		}
		cout << endl;
	}

	return true;
}


//
// Get PC's communication-related settings
//

bool GetPCCommunicationRelatedSettings()
{
	// Communication link can be configured before we connect to the device.
	// No need to connect to the device
	cout << "Using un-connected PvDevice" << endl;
	PvDevice lDevice;

	// Get the communication link parameters array
	cout << "Retrieving communication link parameters array" << endl;
	PvGenParameterArray* lGenLink = lDevice.GetGenLink();

	// Dumping communication link parameters array content
	cout << "Dumping communication link parameters array content" << endl;
	DumpGenParameterArray( lGenLink );

	lDevice.Disconnect();

	return true;
}


//
// Get the GEV Device's settings
//

bool GetIPEngineSettings()
{
	// Connect to the GEV Device.
	PvDevice lDevice;
	PvString lIP;
	if ( !Connect( lDevice, lIP ) )
	{
		return false;
	}

	// Get the device's parameters array. It is built from the 
	// GenICam XML file provided by the device itself
	cout << "Retrieving device's parameters array" << endl;
	PvGenParameterArray* lParameters = lDevice.GetGenParameters();

	// Dumping device's parameters array content
	cout << "Dumping device's parameters array content" << endl;
	DumpGenParameterArray( lParameters );

	// Get width parameter - mandatory GigE Vision parameter, it should be there
	PvGenParameter *lParameter = lParameters->Get( "Width" );

	// Converter generic parameter to width using dynamic cast. If the
	// type is right, the conversion will work otherwise lWidth will be NULL
	PvGenInteger *lWidthParameter = dynamic_cast<PvGenInteger *>( lParameter );

	// Read current width value
	PvInt64 lOriginalWidth = 0;
	if ( !(lWidthParameter->GetValue( lOriginalWidth ).IsOK()) )
	{
		cout << "Error retrieving width from device" << endl;	
		return false;
	}

    // Read max
	PvInt64 lMaxWidth = 0;
	if ( !(lWidthParameter->GetMax( lMaxWidth ).IsOK()) )
	{
		cout << "Error retrieving width max from device" << endl;	
		return false;
	}

	// Change width value
	if ( !lWidthParameter->SetValue( lMaxWidth ).IsOK() )
	{
		cout << "Error changing width on device - the device is on Read Only Mode, please change to Exclusive to change value" << endl;	
		return false;
	} 

	// Reset width to original value
	if ( !lWidthParameter->SetValue( lOriginalWidth ).IsOK() )
	{
		cout << "Error changing width on device" << endl;	
		return false;
	}

	return true;
}


//
// Get Image stream controller settings
//

bool GetImageStreamControllerSettings()
{
	// Connect to the GEV Device.
	PvDevice lDevice;
	PvString lIP;
	if ( !Connect( lDevice, lIP ) )
	{
		return false;
	}

	// Open a Stream
	cout << "Opening stream." << endl;
	PvStream lStream;
	if ( !lStream.Open( lIP ).IsOK() )
	{
		cout << "Error opening stream" << endl;
		return false;
	}

	// Get the stream parameters. These are used to configure/control
	// some stream releated parameters and timings and provides
	// access to statistics from this stream
	cout << "Retrieving streams's parameters array" << endl;
	PvGenParameterArray* lParameters = lStream.GetParameters();

	// Dumping device's parameters array content
	cout << "Dumping stream's parameters array content" << endl;
	DumpGenParameterArray( lParameters );

	lStream.Close();

	return true;
}

//
// Main function
//

int main()
{
	// Communication link parameters display
	cout <<"1. Communication link parameters display" << endl << endl;
	GetPCCommunicationRelatedSettings();
	
	cout << endl;
	// GEV Device parameters display
	cout <<"2. GEV Device parameters display" << endl << endl;
	GetIPEngineSettings();

	cout << endl;
	// Image stream parameters display
	cout <<"3. Image stream parameters display" << endl << endl;
	GetImageStreamControllerSettings();

	cout << endl;
	cout << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

	return 0;
}


