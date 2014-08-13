// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <PvSampleTransmitterConfig.h>


// Default values
#define DEFAULT_LOCAL_ADDRESS ( "" )
#define DEFAULT_HOST_PORT ( 1042 )
#define DEFAULT_MULTICAST_ADDRESS ( "239.192.1.1" )
#define DEFAULT_DEVICE_ADDRESS ( "" )
#define DEFAULT_UNICAST ( false )
#define DEFAULT_CONNECT_DEVICE ( false )
#define DEFAULT_CHANNEL ( 0 )


// Application config
class Config
{
public:

    Config()
    {
        SetDefaults();
    }

    ~Config()
    {
    }

    const string &GetLocalAddress() const { return mLocalAddress; }
    PvUInt16 GetHostPort() const { return mHostPort; }
    const string &GetMulticastAddress() const { return mMulticastAddress; }
    
    const string &GetDeviceAddress() const { return mDeviceAddress; }
    void SetDeviceAddress( const std::string &aAddress ) { mDeviceAddress = aAddress; }

    bool GetUnicast() const { return mUnicast; }
    bool GetConnectDevice() const { return mConnectDevice; }

    PvUInt16 GetChannel() const { return mChannel; }

    void SetDefaults()
    {
        // Set static defaults
        mLocalAddress = DEFAULT_LOCAL_ADDRESS;
        mHostPort = DEFAULT_HOST_PORT;
        mMulticastAddress = DEFAULT_MULTICAST_ADDRESS;
        mDeviceAddress = DEFAULT_DEVICE_ADDRESS;
        mUnicast = DEFAULT_UNICAST;
        mConnectDevice = DEFAULT_CONNECT_DEVICE;
        mChannel = DEFAULT_CHANNEL;
    }

    void ParseCommandLine( int aCount, const char **aArgs )
    {
        if ( ParseOptionFlag( aCount, aArgs, "--help" ) )
        {
            PrintHelp();
            exit( 0 );
        }

        ParseOption<string>( aCount, aArgs, "--localaddress", mLocalAddress );
        ParseOption<PvUInt16>( aCount, aArgs, "--hostport", mHostPort );
        ParseOption<string>( aCount, aArgs, "--multicastaddress", mMulticastAddress );
        ParseOption<string>( aCount, aArgs, "--deviceaddress", mDeviceAddress );

        ParseOptionFlag( aCount, aArgs, "--unicast", &mUnicast );
        ParseOptionFlag( aCount, aArgs, "--connectdevice", &mConnectDevice );

        ParseOption<PvUInt16>( aCount, aArgs, "--channel", mChannel );
    }

    void PrintHelp()
    {
        cout << "This utility receives raw data from a transmitter." << endl;
        cout << "Use this in conjunction with PvTransmitRawSample to illustrate transmitting and receiving raw data using the GigEVision Streaming Protocol (GVSP)." << endl << endl << endl;

        cout << "Optional command line arguments:" << endl << endl;

        cout << "--hostport" << endl;
        cout << "Port to which the device should send." << endl;
        cout << "Default: 1042" << endl << endl;

        cout << "--localaddress" << endl;
        cout << "Address of the interface that is to receive the data." << endl;
        cout << "Default empty - the first interface that can reach the device" << endl << endl;

        cout << "--multicastaddress" << endl;
        cout << "Address to which the device should send and to which the receiver should subscribe." << endl;
        cout << "Default: 239.192.1.1" << endl << endl;

        cout << "--deviceaddress" << endl;
        cout << "Address of the device from which to receive." << endl;
        cout << "Default behaviour opens the device finder window to allow the user to select a device." << endl << endl;

        cout << "--connectdevice" << endl;
        cout << "Initiate a GVCP connection with the device and issue an acquisition start command." << endl;
        cout << "By default, this does not occur and the receiver expects the device to be controled by another entity." << endl << endl;

        cout << "--channel" << endl;
        cout << "Streaming channel to receive from." << endl;
        cout << "Default: 0" << endl << endl;

        cout << "--help \nPrint this help message." << endl;
    }

private:

    string mLocalAddress;
    PvUInt16 mHostPort;
    string mMulticastAddress;
    string mDeviceAddress;

    bool mUnicast;
    bool mConnectDevice;

    PvUInt16 mChannel;
};


#endif // __CONFIGURATION_H__

